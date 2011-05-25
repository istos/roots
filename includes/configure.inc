<?php

define ('ROOTS_EXPORT_NONE', 0x0);
/**
 * @file Abstract Configuration Class
 *
 * THis is an abstract configuration class that provides a base type for
 * other class/functions to use.
 *
 * This is also a base class for ctools to use.
 *
 * @see RootsAdminConfig for example.
 */

abstract class RootsConfigurable {
  // The configuration
  protected $config = array();

  // A unique ID
  protected $id;

  /*
   * Ctools export type for this object
   *
   * Export types can be
   * -EXPORT_IN_DATABASE = the configuration is devined in the database.
   * -EXPORT_IN_CODE = the configuration is defined in code
   * -EXPORT_IN_CODE | EXPORT_IN_DATASE = the configuration is defined in code
   *   but overriden in teh database
   * -ROOTS_EXPORT_NONE = The object is being used only in memory.
   */
  protected $export_type;

  /**
   * Ctools export enabled status
   */
  protected $disabled;

  /**
   * Get the instance of the object
   *
   * @param class
   *   This is the name of the class (ctools class) to retrieve
   * @Param id
   *   This is the unique id to retieve.  This is the key of the table.
   */
  public static function getInstance($class, $id) {
    static $instance = array();
    if (!isset($instance[$class][$id])) {
      $instance[$class][$id] = new $class($id);
    }

    return $instance[$class][$id];
  }

  /**
   * Constructor
   *
   * @param $id
   */
  public function __construct($id = NULL) {
    // Set the ID
    if ($id) {
      $this->id = $id;
      $load = $this->load();
    }
    // If nothing was loaded then fill in the defaults.
    if (!$load) {
      $this->export_type = ROOTS_EXPORT_NONE;
      $this->config = $this->getConfigDefaults();
      $this->disabled = FALSE;
    }
  }

  /**
   * Config Info
   *
   * This returns a an array of the settings and default values.
   * This must be overridden by other classes.
   */
  public function getConfigDefaults() {
    return array();
  }

  /**
   * Save a configuration.  Needs to be implmeneted in extending class
   */
  public abstract function save();

  /**
   * Load a configuration object from the db if necessary.
   *
   * @return boolean
   *   If false, then getConfigDefaults is used.
   */
  protected function load() {
    return FALSE;
  }

  /**
   * Copy a configuration
   */
  public function copy(rootsConfigurable $config) {
    $this->setConfig($config->config);
  }

  /**
   * Set the configuration.
   *
   * The replaces the current configuration.  Be sure to send entire array.
   *
   * @param $config
   *   Array containing configuration info.  Config will be filtered by the keys
   *   returned by getConfigDefaults() and populated with default values that are
   *   not inlcuded in $config
   */
  public function setConfig($config) {
    $defaults = $this->getConfigDefaults();
    $this->config = array_intersect_key($config, $defaults) + $defaults;
  }

  /**
   * Adds to the existing configuration.
   *
   *   Array containing configuration information. Will be filtered by the keys
   *   returned by getConfigDefaults().
   */
  public function addConfig($config) {
    $config = array_merge($this->config, $config);
    $this->setConfig($config);
  }

  /**
   * Override magic method __get(). Make sure that $this->config goes
   * through getConfig()
   */
  public function __get($name) {
    if ($name == 'config') {
      return $this->getConfig();
    }
    return $this->$name;
  }

  /**
   * Get the configuration
   */
  public function getConfig() {
    return $this->config;
  }
}