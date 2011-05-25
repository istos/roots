<?php
// $Id:$

/**
 * @file Node related helper function
 */

/**
 * Get all of the nodes for a ctypes.
 *
 * @param array of types
 * @param optional function name to call for each record.  The $node object is passed.
 *
 * @return array of nodes
 */
function roots_node_process_by_types($types, $function = NULL) {
  if (!empty($function) && !is_callable($function)) {
    return FALSE;
  }

  // If we are passed a string then this is just one type
  if (!is_array($types)) {
    $types[0] = $types;
  }

  // Base sql
  $sql = 'SELECT nid FROM {node} WHERE type in (%s)';
  // Build types string
  $sql_types = db_placeholders($types, 'varchar');
  $args = array(
    'function' => $function,
  );

  $result = db_query($sql, $sql_types);
  roots_include('db');
  $output = roots_db_map($result, 'nid', $args, '_roots_node_process_by_types');

  return $output;
}

/**
 * Helper function for roots_node_get_nodes_by_type() to get an array of nodes
 */
function _roots_node_process_by_types($row, $args) {
  $function = $args['function'];

  $node = node_load($row->nid);
  if (!empty($function)) {
    return call_user_func_array($function, array($node));
  }
  else {
    return $node;
  }
}

/**
 * Get an array of node types.
 * 
 * Usefull for filling in the otpions array of forms.
 * 
 * @param $key
 *  The field to use as the key of the arra.
 * 
 * @param $value
 *  THe field to use as teh value of the array
 */
function roots_node_get_types($key = 'type', $value = 'name') {
  $ctypes = array();
  foreach (node_get_types() as $type) {
    $ctypes[$type->$key] = $type->$value;
  }
  return $ctypes;
}
