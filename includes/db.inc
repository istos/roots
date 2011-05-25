<?php
/**
 * @file Db helper function
 */

/**
 * Run a function every record in a sql query
 *
 * This calls the function and sends the row as a stdClass().
 *
 * @param $result A database query result resource created by db_query
 * @param $function name fo the function
 * @param $key The return array will be keyed by this field
 * @param $params an array of values to pass to the function
 *
 * @return an array of anything returned by the function passed.
 */
function roots_db_map($result, $key = NULL, $params = array(), $function = '_roots_db_array_callback') {
  // If the function isn't callable escape
  if (!is_callable($function)) {
    return FALSE;
  }

  $output = array();

  while($row = db_fetch_object($result)) {
    $args = array('row' => $row, 'args' => $params);
    if (!empty($row->$key)) {
      $output[$key] = call_user_func_array($function, $args);
    }
    else {
      $output[] = call_user_func_array($function, $args);
    }
  }

  return $output;
}

/**
 * Simple callback for roots_db_map().
 *
 * The params array expects an array of values to return.  If only one value
 * is passed then it will be returned not in an array format.  If you add a 'key'
 * as the key of the $params array then it will be used as the key of the array
 */
function _roots_db_array_callback($row, $params) {
  if (empty($params)) {
    return FALSE;
  }

  if (count($params) > 1) {
    $output = array();
    if (empty($params['key'])) {
      unset($params['key']);
      foreach ($params as $field) {
        $output[$params['key']] = $row->$field;
      }
    } else {
      $output[] = $row->$field;
    }
  }
  else {
    $field = $params[0];
    $output = $row->$field;
  }

  return $output;
}