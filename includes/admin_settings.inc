<?php
/**
 * @file Generate Admin Settings Pages and variables
 */

/**
 * @defgroup roots_admin_settings
 * @{
 * Functions to build a dymaic admin page
 *
 * Structure of hook_roots_options().
 *
 * $form array with '#roots_menu' added.
 * All of the settings are optional.  If they are not set, then the $default
 * is used.  The $default is can get set when the roots_admin() class is
 * initialted.
 * @code
 *   roots_include('admin_settings');
 *   $default = array(
 *     'path' => .....,
 *     'title' => .....,
 *     'perm' => ......,
 *   );
 *   $roots_menu = new roots_admin($default, 'admin/settings/module');
 * @endcode
 *
 * $form['item']['#roots_menu''] = array(
 *  'title' => ...,
 *  'perm' => array('permission'),
 *  'description' => ....,
 *  'weight' => 0
 * )
 * Example Usage
 * @code
 *
 * //Implementation of hook_menu
 *
 *function superman_menu() {
 *   // Initialize roots_menu
 *   roots_include('admin_settings');
 *   $default_tab = array(
 *     'path' => 'superman_general',
 *     'title' => 'Settings for Waiting for Superman',
 *     'perm' => array('admin superman'),
 *   );
 *   $roots_menu = new roots_admin($default_tab, 'admin/settings/superman');
 *
 *   $items = $roots_menu->getMenu();
 *
 *   return $items;
 * }
 *
 * // Implementation of hook_roots_options()
 * function superman_roots_options() {
 *   $ctypes = array();
 *   foreach (node_get_types() as $type) {
 *     $ctypes[$type->type] = $type->name;
 *   }
 *   $items = array();
 *   $items['superman_context_ctypes'] = array(
 *     '#type' => 'select',
 *     '#options' => $ctypes,
 *     '#title' => t('Context Content Types'),
 *     '#description' => t('Content Types that will create a context per node.'),
 *     '#default_value' => array(),
 *     '#multiple' => TRUE,
 *   );
 *
 *    return $items;
 *  }
 * @endcode
 */

class roots_admin {
  // Default tab
  // array('path' => $path, 'title' => $title)
  public $default_tab = '';
  public $base_path = '';
  protected $menu = array();
  // flat array used for retrieval
  protected $form = array();
  // cache format
  // array(
  //   'default_tab' => array('path, ''titile', 'perm'),
  //   'base_path')
  private static $cache_name = 'roots_admin_cache';

  /**
   * Constructor
   */
  function  __construct($default_tab = array(), $base_path = array()) {
    ctools_include('object-cache');
    $cache = ctools_object_cache_get('roots', self::$cache_name);
    $this->default_tab = $cache['default_tab'];
    $this->base_path = $cache['base_path'];

    $this->default_tab = !empty($default_tab) ? $default_tab : $this->default_tab;
    $this->base_path = !empty($base_path) ? $base_path : $this->base_path;

    // Build the form
    $this->formBuilder();
  }

  /**
   * Destruct
   */
  function  __destruct() {
    ctools_include('object-cache');
    $cache = array(
      'default_tab' => $this->default_tab,
      'base_path' => $this->base_path,
    );
    ctools_object_cache_set('roots', self::$cache_name, $cache);
  }

  /**
   * Getter and Setter
   */
  public function getValue($key, $default = NULL) {
    if (!empty($default)) {
      return variable_get($key, $default);
    }
    $default = $this->form[$key]['#default_value'];
    return variable_get($key, $default);
  }

  public function setValue($key, $value) {
    varible_set($key, $value);
  }

  /**
   * Get a form
   */
  public function getForm($page = NULL) {
    // if no page is passed then use the default
    $page = empty($page) ? $this->default_tab['path'] : $page;

    return $this->menu[$page]['form'];
  }

  /**
   * Get the menu items
   */
  public function getMenu() {
    $items = array();
    $default = $this->default_tab;

    // Add the main page callback
    $items[$this->base_path] = array(
      'type' => MENU_NORMAL_ITEM,
      'title' => $default['title'],
      'description' => !empty($default['description']) ? $default['description'] : '',
      'page callback' => 'drupal_get_form',
      'file' => 'admin_settings.inc',
      'file path' => drupal_get_path('module', 'roots') . '/includes',
      'access arguments' => $default['perm'],
      'page arguments' => array('roots_admin_form', $default['path']),
    );
    // Add default tab
    $items[$this->base_path . '/' . $default['path']] = array(
      'type' => MENU_DEFAULT_LOCAL_TASK,
      'title' => $default['title'],
      'description' => !empty($default['description']) ? $default['description'] : '',
      'weight' => -50,
    );
    foreach ($this->menu as $item) {
      $items[$this->base_path . '/' . $item['path']] = array(
        'type' => MENU_LOCAL_TASK,
        'title' => !empty($item['title']) ? $item['title'] : $default['title'],
        'page callback' => 'drupal_get_form',
        'file' => 'admin_settings.inc',
        'file path' => drupal_get_path('module', 'roots') .'/includes',
        'access arguments' => !empty($item['perm']) ? $item['perm'] : $default['perm'],
        'page arguments' => array('roots_admin_form', $item['path']),
        'weight' => !empty($item['weight']) ? $item['weight'] : 0,
        'description' => !empty($item['description']) ? $item['description'] : '',
      );
    }
    return $items;
  }

  /**
   * Form builder.
   *
   * Invokes hook_roots_options().
   */
  public function formBuilder() {
    $options = $this->getOptions();
    $form = array();
    foreach ($options as $field_name => $field) {
      $this->_formBuilder($options, $field_name);
    }
  }

  /**
   * Helper recursive function to build form
   */
  private function _formBuilder(&$settings, $field_name) {
    // If this is a fieldset then go to the next level down
    // @TODO fieldset is not rendering.  USing it breaks the site
    if ($settings[$field_name]['#type'] == 'fieldset') {
      // Add the fieldset to the array
      $this->form[$field_name] = element_properties($settings[$field_name]);
      $this->_formBuilder($settings[$field_name], $field_name);
    }
    else {
      // If the type is empty then use checkbox
      if (empty($settings[$field_name]['#type'])) {
        $settings[$field_name]['#type'] = 'checkbox';
      }

      // Set the tab.  If there is none then use the default
      $menu_tab = !empty($settings[$field_name]['#roots_menu'])
        ? $settings[$field_name]['#roots_menu']
        : $this->default_tab;

      // Remove the #roots_menu key
      unset($settings[$field_name]['#roots_menu']);

      // Set the default
      $default = $this->getValue($field_name, $settings[$field_name]['#default_value']);
      $settings[$field_name]['#default_value'] = $default;

      // Add to the variables
      foreach ($menu_tab as $idx => $v) {
        $this->menu[$menu_tab['path']][$idx] = $v;
      }
      $this->menu[$menu_tab['path']]['form'][$field_name] = $settings[$field_name];
      $this->form[$field_name] = $settings[$field_name];
    }
  }

  /**
   * Get the options
   *
   * Invoke hook_roots_options()
   */
  private function getOptions() {
    $options = &roots_static('roots_admin_options');
    if (empty($options)) {
      $options = module_invoke_all('roots_options');
    }
    return $options;
  }
}

/**
 * @} end of group roots_admin_settings
 */

/**
  * Admin form
  */
function roots_admin_form($form_state, $page) {
  $roots_form = new roots_admin();
  $form = $roots_form->getForm($page);
  return system_settings_form($form);
}