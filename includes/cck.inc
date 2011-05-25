<?php

/**
 * @file
 * Functions to help with cck
 */

/**
 * Get the table and columns for a cck field.
 *
 * @param string $field_name
 *   cck field name
 * @return array
 *   The return array contains table and column information for the field.
 *   This information is different depending on what type of field it is.
 *   For example, the date field type has a from date and a to date.
 */
function roots_cck_get_field_db_info($field_name) {
  $db_info = &roots_static(__FUNCTION__);

  if (empty($db_info[$field_name])) {
    if (!isset($db_info)) {
      $db_info = array();
    }
    $field = content_fields($field_name);
    $db_info[$field_name] = content_database_info($field);
  }

  return $db_info[$field_name];
}

/**
 * Get all of the field object for a certain type.
 *
 * @param string type name
 * @param optional content type
 *
 * @return array of fields
 */
function roots_cck_get_field_by_type($type) {
  $return = &roots_static(__FUNCTION__);

  if (empty($return[$type])) {
    $types = _content_field_types();

    if (empty($types[$type])) {
      return array();
    }

    $fields = content_fields();
    foreach($fields as $field) {
      if ($field['type'] == $type) {
        $return[$type][$field['field_name']] = $field;
      }
    }

  }
  return $return[$type];
}