<?php

/**
 * Delegated hook_profile_tasks() implementation.
 */
function roots_install_profile_tasks(&$task, $url, $modules = array()) {
  if ($task == 'profile') {
    $batch = array(
      'title' => st('Installing modules'),
      'operations' => array(),
      'finished' => 'roots_profile_modules_batch_finished',
      'file' => drupal_get_path('module', 'roots') .'/includes/install.inc',
    );

    roots_include('module');
    $modules = roots_module_dependencies($modules);
    $files = module_rebuild_cache();
    foreach ($modules as $module) {
      $batch['operations'][] = array('_install_module_batch', array($module, $files[$module]->info['name']));
    }

    variable_set('install_task', 'roots-install-modules-batch');

    batch_set($batch);
    batch_process($url, $url);
    // batch_process exits.
  }

  // We are running a batch install of the profile's modules.
  // This might run in multiple HTTP requests, constantly redirecting
  // to the same address, until the batch finished callback is invoked.
  // and the task advances to 'profile-install-modules-finished'.
  if ($task == 'roots-install-modules-batch') {
    include_once 'includes/batch.inc';
    return _batch_page();
  }

  if ($task == 'roots-install-modules-finished') {
    $task = 'roots-finished';
  }
}

/**
 * Finished callback for the modules install batch.
 *
 * @see roots_install_profile_tasks().
 */
function roots_profile_modules_batch_finished($success, $results) {
  // Signal to the installer that the batch is finished.
  variable_set('install_task', 'roots-install-modules-finished');
}

