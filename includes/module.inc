<?php

/**
 * @file
 * Module related functions.
 */

/**
 * Recursively determine module dependencies.
 *
 * @param $modules
 *   An array of module names.
 * @return
 *   Returns an ordered list of module dependencies, including the modules
 *   from the original list.
 */
function roots_module_dependencies($modules) {
  static $all_modules;
  if (!isset($all_modules)) {
    $all_modules = module_rebuild_cache();
  }
  $return = array();

  foreach ($modules as $m) {
    $dependencies = $all_modules[$m]->info['dependencies'];
    if (is_array($dependencies) && count($dependencies)) {
      $dependencies = roots_module_dependencies($dependencies);
      foreach ($dependencies as $d) {
        if (!isset($return[$d])) {
          $return[$d] = $d;
        }
      }
    }
    $return[$m] = $m;
  }

  return array_values($return);
}

/**
 * Get info for system object
 *
 * @param $type
 * -module
 * -theme
 * @param $name
 *   Name of module or theme
 * @param $reset
 *   Reset the cache
 */
function roots_module_get_info($type = NULL, $name = NULL, $reset = FALSE) {
  static $info;
  if (!isset($info) || $reset) {
    $result = db_query('SELECT name, type FROM {system}');
    while ($row = db_fetch_object($result)) {
      $info[$row->type][$row->name] = unserialize($row->info);
    }
  }
  if (isset($type, $name)) {
    return isset($info[$type][$name]) ? $info[$type][$name] : FALSE;
  }
  else if (isset($type)) {
    return isset($info[$type]) ? $info[$type] : FALSE;
  }
  else {
    return $info;
  }

}

/**
 * Uninstall a list of modules.
 *
 * This runs the disable and uninstall functions
 *
 * @param array list of modules
 */
function roots_module_uninstall_modules(array $modules) {
  if (!empty($modules)) {
    module_disable($modules);

    foreach ($modules as $module) {
      drupal_uninstall_module($module);
    }
  }
}
