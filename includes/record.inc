<?php

// $ID:$

/**
 * @file
 * Record abstract class
 */

abstract class RootsRecord {


  // Internal variables.
  protected $_data = array();
  protected $_valid;
  protected $_needs_validation = TRUE;
  protected $_errors = array();

  // Run the unserialize
  protected $_should_unserialize = FALSE;

  // Define the table this record is for
  abstract protected function table();

  // Define any unique fields
  protected function unique_fields() {
    return array();
  }

  /**
   * Public method to get the fields
   */
  public function getFields() {
    $schema = drupal_get_schema($this->getTable());
    return array_keys($schema['fields']);
  }

  /**
   * Get the unique fields
   */
  public function getUniqueFields() {
    $unique_keys = array();

    // Check the schema API
    $schema = drupal_get_schema($this->getTable());
    $schema_unique_keys = isset($schema['unique keys']) ? $schema['unique keys'] : array();
    foreach ($schema_unique_keys as $keys) {
      $unique_keys = array_merge($unique_keys, $keys);
    }

    return array_merge($unique_keys, $this->unique_fields());
  }

  /**
   * Public method to check if a field is serialized
   */
  public function is_serialized($field) {
    $schema = drupal_get_schema($this->getTable());
    return isset($schema['fields'][$field]['serialize']) ? $schema['fields'][$field]['serialize'] : FALSE;
  }

  /**
   * Public method to the table
   */
  public function getTable() {
    return $this->table();
  }

  /**
   * Constructor for Record objects.
   *
   * @param $param
   *   Can be either an id or an array.
   */
  public function __construct($param = NULL) {
    if (!empty($param)) {
      if (is_numeric($param)) {
        $this->load($param);
      }
      elseif (is_array($param)) {
        $this->load_from_array($param);
      }
    }
  }

  /**
   * Load the record object from the database row identified by $id.
   */
  public function load($id) {
    $row = db_fetch_array(db_query('SELECT * FROM {'. $this->table() .'} WHERE id = %d', $id));
    if (!empty($row)) {
      $this->_data = $row;
    }
    if ($this->_should_unserialize) {
      $this->unserialize();
    }
  }

  /**
   * Unserialze all serilzed fields
   */
  protected function unserialize() {
    foreach ($this->_data as $field_name => $field) {
      if ($this->is_serialized($field_name)) {
        $this->_data[$field_name] = unserialize($field);
      }
    }
  }

  /**
   * Load the record object with data from an array.
   */
  public function load_from_array($array) {
    $fields = $this->getFields();
    foreach ($array as $key => $value) {
      if (in_array($key, $fields)) {
        $this->_data[$key] = $value;
      }
    }
    $this->invalidate();
  }

  /**
   * Is the record new?
   */
  public function is_new() {
    return (empty($this->id));
  }

  /**
   * Does the record pass validation?
   */
  public function is_valid() {
    if ($this->_needs_validation) {
      $this->reset_errors();
      $this->_valid = $this->validate();
      $this->_needs_validation = FALSE;
    }
    return $this->_valid;
  }

  /**
   * Validation routine for the record. Should be overridden by sub-classes.
   */
  protected function validate() {
    return TRUE;
  }

  /**
   * Mark the record as needing validation.
   */
  protected function invalidate() {
    $this->_needs_validation = TRUE;
  }

  /**
   * Returns the array of validation errors.
   */
  public function get_errors() {
    return $this->_errors;
  }

  /**
   * Used internally to set a validation error.
   */
  protected function set_error($error, $field = 'all') {
    $this->_errors[$field][] = $error;
  }

  /**
   * Resets the errors array.
   */
  protected function reset_errors() {
    $this->_errors = array();
  }

  /**
   * Check for uniqueness
   *
  protected function checkUniqueFields() {
    $unique_fields = $this->getUniqueFields();
    if (empty($unique_fields)) {
      return TRUE;
    }

    $fields = $this->getFields();

    $where = array();
    $where_value = array();
    foreach ($unique_fields as $field) {
      if (empty($this->$field)) {
        $this->_valid = FALSE;
        $this->_needs_validation = FALSE;
        return FALSE;
      }

      if (in_array($fields[$field]['type'], array('int', 'float', 'numeric'))) {
        $where[] = "$field = %d";
      }
      else {
        $where[] = "$field = '%s'";
      }
      $where_value[] = $this->$field;
      
    }

    $sql = "SELECT id FROM {" . $this->table() . "} WHERE " . implode(' AND ', $where);

    $model_name = get_class($this);
    $model = new $model_name();
    return RootsFind::find_by($sql, $where_value, $model);
  }*/

  /**
   * Save the record to the database.
   */
  public function save() {
    //$current = $this->checkUniqueFields();

    if (!$this->is_valid()) {
      return FALSE;
    }

    if ($this->is_new()) {
      $return = drupal_write_record($this->table(), $this);
      $this->_data['id'] = db_last_insert_id($this->table(), 'id');
    }
    else {
      $return = drupal_write_record($this->table(), $this, array('id'));
    }

    $this->invalidate();
    return $return;
  }

  /**
   * Delete the record from the database.
   */
  public function delete() {
    db_query('DELETE FROM {'. $this->table() .'} WHERE id = %d', $this->id);
    unset($this->_data['id']);
  }

  /**
   * Property overloading for setting the value of a db column. The 'id' field
   * is not settable through the public interface.
   */
  public function __set($name, $value) {
    if (($name != 'id') && in_array($name, $this->getFields())) {
      $this->_data[$name] = $value;
      $this->invalidate();
    }
  }

  /**
   * Property overloading for getting the value of a db column.
   */
  public function __get($name) {
    if (array_key_exists($name, $this->_data)) {
      return $this->_data[$name];
    }
    return NULL;
  }

  /**
   * Property overloading for testing is a db column field is set.
   */
  public function __isset($name) {
    return isset($this->_data[$name]);
  }

  /**
   * Property overloading for unsetting a db column field.
   */
  public function __unset($name) {
    if (($name != 'id') && in_array($name, $this->getFields())) {
      unset($this->_data[$name]);
      $this->invalidate();
    }
  }

  /**
   * Create new record
   *
   * @see __construct for more info
   */
  public static function create($param, $class) {
    //if ($class instanceof self) {
      $record = new $class($param);
      $record->save();
      return $record;
    //}

    //return 'Cool';
  }
}

/**
 * Find class
 */

class RootsFind {

  /**
   * Return all of the records by one field
   *
   * @static
   * @param RootsRecord $record
   * @param  $field
   * @return array of RootsRecord
   */
  public static function find_by_string($field, $value, RootsRecord &$record) {
    if (in_array($field, $record->getFields())) {
      $sql = 'SELECT id FROM {' . $record->getTable() . '} WHERE ' . $field . " = '%s'";
      return self::find_by($sql, array($value), $record);
    }
  }

  public static function find_by_number($field, $value, RootsRecord &$record) {
    if (in_array($field, $record->getFields())) {
      $sql = 'SELECT id FROM {' . $record->getTable() . '} WHERE ' . $field . ' = %d';
      return self::find_by($sql, $value, $record);
    }
  }

  public static function find_all(RootsRecord &$record) {
    $sql = 'SELECT id FROM {' . $record->getTable() . '}';
    return self::find_by($sql, array(), $record);
  }

  public static function find_by($sql, $args, RootsRecord &$record) {
    $class = get_class($record);
    $return = array();
    $result = db_query($sql, $args);
    while ($row = db_fetch_object($result)) {
      $return[$row->id] = new $class($row->id);
    }

    return $return;
  }
}