<?php

// $Id:$

/**
 * @file
 * Helper functions for caching
 */

/**
 * Get the actual cache value
 *
 * @param $cid cache id
 * @param $table
 *  defaults to 'table'
 */
function roots_cache_get($cid, $table = 'cache') {
  $cache_obj = cache_get($cid, $table);
  if ($cache_obj) {
    $value = $cache_obj->data;
  }

  return $value;
}