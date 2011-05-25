<?php

/**
 * @file Generic PHP functions
 */

/**
 * Convert a multi-dimensional array into an object
 */
function roots_arrayToObject($array) {
	if(!is_array($array)) {
		return $array;
	}
	$object = new stdClass();
	if (is_array($array) && count($array) > 0) {
	  foreach ($array as $name=>$value) {
	     $name = strtolower(trim($name));
	     if (!empty($name)) {
	        $object->$name = arrayToObject($value);
	     }
	  }
      return $object;
	}
    else {
      return FALSE;
    }
}

/**
 * Decode json into an nexted arrays instead of objects
 *
 * @param <strong> of json
 */
function roots_php_decode_json($json) {
  //remove curly brackets to beware from regex errors
  $json = substr($json, strpos($json,'{')+1, strlen($json));
  $json = substr($json, 0, strrpos($json,'}'));
  // This changes the output to arrays instead of objects
  $json = preg_replace('/(^|,)([\\s\\t]*)([^:]*) (([\\s\\t]*)):(([\\s\\t]*))/s', '$1"$3"$4:', trim($json));

  return json_decode('{'.$json.'}', true);
}