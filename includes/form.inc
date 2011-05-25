<?php
/**
 * @file form helper function
 */

/**
 * Get an array of options to use in a form #options array
 *
 * @param $result return from db_query().
 * @param $key_field field to use as the array key
 * @param $value_field to use as the value
 *
 * @return an array of ($key => $value)
 */
function roots_form_options_builder($result, $key_field, $value_field) {
  roots_include('db');

  $output = roots_db_map($result, $key_field, array($value_field));

  return $output;
}