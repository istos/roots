<?php
/**
 * @file
 * This is an example install profile using the roots install utilities.
 */

/**
 * Implementation of hook_profile_modules().
 */
function example_profile_modules() {
  // Only list core modules + roots here.
  return array('help', 'menu', 'path', 'taxonomy', 'dblog', 'comment', 'roots');
}

/**
 * Implementation of hook_profile_details().
 */
function example_profile_details() {
  return array(
    'name' => 'Example',
    'description' => 'Custom roots-based installer.',
  );
}

/**
 * Implementation of hook_profile_tasks().
 */
function example_profile_tasks(&$task, $url) {
  // All other modules should be listed here. Dependencies will be picked up
  // automatically, so don't worry about listing them all here.
  $modules = array(
    'example_feature',
  );

  roots_include('install');

  $output = roots_install_profile_tasks($task, $url, $modules);
  if (!empty($output)) {
    return $output;
  }

  if ($task == 'roots-finished') {
    // Do extra stuff here if needed. Make sure to advance the task to
    // 'profile-finished' when done.
    $task = 'profile-finished';
  }
}
