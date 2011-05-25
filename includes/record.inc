<?php

// $ID:$

/**
 * @file
 * Record abstract class
 */

abstract class RootsRecord {


  // Internal variables.
  protected $data = array();
  protected $valid;
  protected $needsValidation = TRUE;
  protected $errors = array();

  // Run the unserialize
  protected $shouldUnserialize = FALSE;

  // Define the table this record is for
  abstract protected function table();

  /**
   * Public method to get the fields
   */
  public function getFields() {
    $schema = drupal_get_schema($this->getTable());
    return array_keys($schema['fields']);
  }

  /**
   * Public method to check if a field is serialized
   */
  public function isSerialized($field) {
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
        $this->loadFromArray($param);
      }
    }
  }

  /**
   * Load the record object from the database row identified by $id.
   */
  public function load($id) {
    $row = db_fetch_array(db_query('SELECT * FROM {'. $this->table() .'} WHERE id = %d', $id));
    if (!empty($row)) {
      $this->data = $row;
    }
    if ($this->shouldUnserialize) {
      $this->unserialize();
    }
  }

  /**
   * Unserialze all serilzed fields
   */
  protected function unserialize() {
    foreach ($this->data as $field_name => $field) {
      if ($this->isSerialized($field_name)) {
        $this->data[$field_name] = unserialize($field);
      }
    }
  }

  /**
   * Load the record object with data from an array.
   */
  public function loadFromArray($array) {
    $fields = $this->getFields();
    foreach ($array as $key => $value) {
      if (in_array($key, $fields)) {
        $this->data[$key] = $value;
      }
    }
    $this->invalidate();
  }

  /**
   * Is the record new?
   */
  public function isNew() {
    return (empty($this->id));
  }

  /**
   * Does the record pass validation?
   */
  public function isValid() {
    if ($this->needsValidation) {
      $this->resetErrors();
      $this->valid = $this->validate();
      $this->needsValidation = FALSE;
    }
    return $this->valid;
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
    $this->needsValidation = TRUE;
  }

  /**
   * Returns the array of validation errors.
   */
  public function getErrors() {
    return $this->errors;
  }

  /**
   * Used internally to set a validation error.
   */
  protected function setError($error, $field = 'all') {
    $this->errors[$field][] = $error;
  }

  /**
   * Resets the errors array.
   */
  protected function resetErrors() {
    $this->errors = array();
  }

  /**
   * Save the record to the database.
   */
  public function save() {
    //$current = $this->checkUniqueFields();

    if (!$this->isValid()) {
      return FALSE;
    }

    if ($this->isNew()) {
      $return = drupal_write_record($this->table(), $this);
      $this->data['id'] = db_last_insert_id($this->table(), 'id');
    }
    else {
      $return = drupal_write_record($this->table(), $this, array('id'));
    }

    $this->invalidate();
    return $this->saveCheck();
  }

  /**
   * Check to see if the record was loaded
   */
  protected function saveCheck() {
    if (empty($this->data['id'])) {
      $this->errors[] = t('Record Not Saved');
      return FALSE;
    }

    return TRUE;
  }

  /**
   * Delete the record from the database.
   */
  public function delete() {
    db_query('DELETE FROM {'. $this->table() .'} WHERE id = %d', $this->id);
    unset($this->data['id']);
  }

  /**
   * Property overloading for setting the value of a db column. The 'id' field
   * is not settable through the public interface.
   */
  public function __set($name, $value) {
    if (($name != 'id') && in_array($name, $this->getFields())) {
      $this->data[$name] = $value;
      $this->invalidate();
    }
  }

  /**
   * Property overloading for getting the value of a db column.
   */
  public function __get($name) {
    if (array_key_exists($name, $this->data)) {
      return $this->data[$name];
    }
    return NULL;
  }

  /**
   * Property overloading for testing is a db column field is set.
   */
  public function __isset($name) {
    return isset($this->data[$name]);
  }

  /**
   * Property overloading for unsetting a db column field.
   */
  public function __unset($name) {
    if (($name != 'id') && in_array($name, $this->getFields())) {
      unset($this->data[$name]);
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
 * The Exception Class
 */
class RootsException extends Exception {}

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
  public static function findByString($field, $value, RootsRecord &$record) {
    if (in_array($field, $record->getFields())) {
      $sql = 'SELECT id FROM {' . $record->getTable() . '} WHERE ' . $field . " = '%s'";
      return self::findBy($sql, array($value), $record);
    }
  }

  public static function findByNumber($field, $value, RootsRecord &$record) {
    if (in_array($field, $record->getFields())) {
      $sql = 'SELECT id FROM {' . $record->getTable() . '} WHERE ' . $field . ' = %d';
      return self::findBy($sql, $value, $record);
    }
  }

  public static function findAll(RootsRecord &$record) {
    $sql = 'SELECT id FROM {' . $record->getTable() . '}';
    return self::findBy($sql, array(), $record);
  }

  public static function findBy($sql, $args, RootsRecord &$record) {
    $class = get_class($record);
    $return = array();
    $result = db_query($sql, $args);
    while ($row = db_fetch_object($result)) {
      $return[$row->id] = new $class($row->id);
    }

    return $return;
  }
}

