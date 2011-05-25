<?php

// $Id:$

/**
 * @file
 *  functions to run other functions
 */

/**
 * Generic Get function
 *
 * @param $get_function The name of the function to call.  The helper function should not return NULL
 *   YOu can also call a class and method by passing an array('class_name', 'method').  @See call_user_func_array
 * @param $args an array of argumments that will be passed to the function
 * @param $allow_alter defaults to TRUE
 * @param $reset reset the static cache
 * @param $cache cache using cache_get.
 *  If this is an array then the keys should be 'cid' and 'table'
 *  If this is an boolean, it will use the function as the cid and the default cache table.
 *
 * @return the output from the function or NULL if there was an error
 */
function roots_get($get_function, $args = array(), $allow_alter = TRUE, $reset = FALSE, $cache = FALSE) {
  $values = &roots_static(__FUNCTION__);

  if (!isset($values)) {
    $values = array();
  }

  // If the functin is an array, ie and class::method concat them
  if (is_array($get_function)) {
    $cache_id = implode('::', $get_function);
  }
  else {
    $cache_id = $get_function;
  }

  // If you reset the cache or the value is not staticly cached
  if ($reset || !isset($values[$cache_id])) {
    if ($cache && !$reset) {
      if (is_array($cache)) {
        $cid = isset($cache['cid']) ? $cache['cid'] : $cache_id;
        $cache_table = isset($cache['table']) ? $cache['table'] : 'cache';
      }
      else {
        $cid = $get_function;
        $cache_table = 'cache';
      }

      roots_include('cache');
      $value = roots_cache_get($cid, $cache_table);

      if (is_null($value)) {
        $value = roots_run($get_function, $args, $allow_alter);
      }
    }
    else {
      $value = roots_run($get_function, $args, $allow_alter);
    }

    // Set the cache
    if ($cache) {
      if (is_array($cache)){
        $expire = isset($cache['expire']) ? $cache['expire'] : CACHE_PERMANENT;
      }
      else {
        $expire = CACHE_PERMANENT;
      }

      cache_set($cid, $value, $cache_table, $expire);
    }

    $values[$cache_id] = $value;
  }

  return $values[$cache_id];
}

/**
 * Run a function and call an alter on the output.
 *
 * No caching built in, just runs code.  Use the @see roots_get
 */
function roots_run($get_function, $args = array(), $allow_alter = TRUE) {
  if (!is_callable($get_function)) {
    return NULL;
  }
  $output = call_user_func_array($get_function, $args);

  if ($allow_alter) {
   $output = drupal_alter($get_function, $output);
  }

  return $output;
}