<?php
// $Id:$

/**
 * @file
 * Functions related to input formats and filters
 */

/**
 * Save Input Formats
 *
 * Save input formats to the database.  Updating currenting ones and crateing new one.
 * Filter specific settings are also saved per the variable format.
 *
 * Here is an exmple hook_update_n
 *
 function example_update_6001() {
  $input_formats = array();

  $input_format = new stdClass;
  $input_format->name = 'Filtered HTML';
  $input_format->roles = array(
    // An array of role names.  This is matched against the db
    'anonymous user',
    'authenticated user',
  );
  // If the filters all allow caching then also cache the format
  $input_format->cache = TRUE;
  $input_format->settings = array(
    // Settings for: filter module
    'filter' => array(
      // Filter HTML tags
      // Variable name is filter_html_1
      'filter_html' => 1,
    ),
  );
  $input_format->filters = array(
    'filter' => array(
      // Delta => Weight for: URL filter
      '2' => '0',
      // Delta => Weight for: HTML filter
      '0' => '1',
      // Delta => Weight for: Line break converter
      '1' => '2',
      // Delta => Weight for: HTML corrector
      '3' => '10',
    ),
  );
  $input_formats['filtered_html'] = $input_format;

  $input_format = new stdClass;
  $input_format->name = 'Full HTML';
  $input_format->roles = array(
    'authenticated user',
    'administrator',
  );
  $input_format->cache = TRUE;
  $input_format->settings = array(
  );
  $input_format->filters = array(
    'filter' => array(
      // Delta => Weight for: URL filter
      '2' => '0',
      // Delta => Weight for: Line break converter
      '1' => '1',
      // Delta => Weight for: HTML corrector
      '3' => '10',
    ),
    'video_filter' => array(
      // Delta => Weight for: Video Filter
      '0' => '10',
    ),
  );
  $input_formats['full_html'] = $input_format;

  // Include the file
  roots_include('format');
  // Save the formats
  roots_format_save($input_formats);


}
 *
 * @param array of input formats
 */
function roots_format_save(array &$input_formats) {
  if (!empty($input_formats)) {
    foreach($input_formats as $input_format) {
      // Can we use cache
      $cache = roots_format_can_cache($input_format);
      $roles = roots_format_get_compatible_roles($input_format);
      $roles = ','. implode(',', $roles) .',';

      // First check to see if the format exists
      if ($cur_format = roots_format_get_format_by_name($input_format->name)) {
        // We are updating
        $cur_format['roles'] = $roles;
        $cur_format['cache'] = $cache;
        drupal_write_record('filter_formats', $cur_format, array('format'));

        $input_format->format = $cur_format['format'];
        // Remove the old formats
        db_query("DELETE FROM {filters} WHERE format = %d", $input_format->format);
      } else {
        $new_format = new stdClass();
        $new_format->name = $input_format->name;
        $new_format->roles = $roles;
        $new_format->cache = $cache;
        drupal_write_record('filter_formats', $new_format);
        $input_format->format = $new_format->format;
      }

      // Save the filters
      if (!empty($input_format->filters)) {
        foreach($input_format->filters as $module => $filter) {
          $filters = module_invoke($module, 'filter', 'list');
          foreach ($filter as $delta => $weight) {
            // Validate that the filter exists in code
            if (isset($filters[$delta])) {
              $filter_save = new stdClass();
              $filter_save->module = $module;
              $filter_save->delta = $delta;
              $filter_save->weight = $weight;
              $filter_save->format = $input_format->format;
              drupal_write_record('filters', $filter_save);
            }
          }

          // Add settings
          roots_format_save_settings($input_format);

          // Set the default
          if (!empty($input_foramt->default) && $input_format->default) {
            variable_set('filter_default_format', $filter_save->format);
          }
        }
      }
      // Clear the cache
      cache_clear_all($input_format->format .':', 'cache_filter', TRUE);
    }
  }
}

/**
 * Determine wich roles can use this input format.
 *
 * When a input format is installed in a site, some roles can not be
 * available in the site. This function determine wich roles are exactly
 * equals to defined by code comparing a non translated version of the
 * roles.
 *
 * If a role doens't exists, a warning message will be displayed in the
 * site.
 *
 * @param input format object
 * @return array for rids
 */
function roots_format_get_compatible_roles($input_format) {
  roots_include('user');
  $roles = array();
  foreach ($input_format->roles as $role_name) {
    if ($role_id = roots_user_get_role_id($role_name)) {
      $roles[] = $role_id;
    }
  }

  return $roles;
}

/**
 * Determine if a input format can cache their process based in the filters
 * that it loads from code.
 */
function roots_format_can_cache($input_format) {
  $cache = TRUE;
  foreach ($input_format->filters as $module => $filter) {
    foreach ($filter as $delta => $weight) {
      $cache &= !module_invoke($module, 'filter', 'no cache', $delta);
    }
  }
  return $cache;
}

/**
 * Get a input format by name
 *
 * @param $name name of the filter format
 */
function roots_format_get_format_by_name($name) {
  $name = strtolower($name);
  $formats = &roots_static(__FUNCTION__);

  if (empty($formats[$name])) {
    $format = db_fetch_array(db_query('SELECT * FROM {filter_formats} WHERE LOWER(name) = "%s"', $name));
    if (!empty($format['format'])) {
      $formats[$name] = $format;
    }
    else {
      $formats[$name] = array();
    }
  }

  return $formats[$name];
}

/**
 * Simulate the submission of admin/setting/filters/$format/configure
 *
 * When a new input format is created, settings are not saved into the
 * database.  This saves the settings to the vairables
 *
 * @param $format
 *   A format object returned by filter_formats function.
 */
function roots_format_save_settings($format) {
  if (!empty($format->settings) && is_array($format->settings)) {
    foreach ($format->settings as $module => $filter) {
      foreach ($filter as $variable_name => $variable_value) {
        $variable_name = $variable_name . '_' . $format->format;
        variable_set($variable_name, $variable_value);
      }
    }
  }
}
