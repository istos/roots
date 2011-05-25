<?php
/**
 * @file
 * User help functions
 */

/**
 * Add a permisison for a user
 *
 * @param string $role
 * @param string $perm
 */
function roots_user_add_perm($role, $perm) {
  $perms = roots_user_get_perms($role);

  $perms[] = $perm;
  $perm_string = implode(',', $perms);

  roots_user_set_perm($role, $perm_string);
}

/**
 * Save permissions
 *
 * @param string $role
 * @param string $perms
 */
function roots_user_set_perm($role, $perms) {
  $rid = roots_user_get_role_id($role);

  $record = new stdClass();
  $record->rid = $rid;
  $record->perm = $perms;

  drupal_write_record('permission', $record);
}

/**
 * Get the permissions for a role
 *
 * @param string $role
 * @return array of roles
 */
function roots_user_get_perms($role) {
  $perms = &roots_static(__FUNCTION__);

  if (empty($perms[$role])) {
    $rid = roots_user_get_role_id($role);

    $perm = db_result(db_query('SELECT perm FROM {permission} WHERE rid = %d', $rid));

    $perms[$role]  = explode(',', $perm);
  }

  return $perms[$role];
}

/**
 * Get the role id for a role
 *
 * @param string $role
 * @return int
 */
function roots_user_get_role_id($role) {
  $role = strtolower($role);
  $rids = &roots_static(__FUNCTION__);

  if (empty($rids[$role])) {
    $rids[$role] = db_result(db_query('SELECT rid FROM {role} WHERE LOWER(name) = "%s"', $role));
  }

  return $rids[$role];
}

/**
 * Returns the user roles without translate role names.
 */
function roots_user_raw_user_roles() {
  $user_roles = array();
  $result = db_query("SELECT rid, name FROM {role}");
  roots_include('form');
  $user_roles = roots_form_options_builder($result, 'rid', 'name');
  return $user_roles;
}