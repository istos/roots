<?php
// $ID:$

/**
 * @file
 *  THis is a example implementation of the record class.
 *
 *  You should create your schema as normal in hook_schema.  The Roots_Record class
 *  is designed to work with a single table.  The only requirement is that you have a
 *  serial id field as the primary key.
 *
 *  It's good practice to name your table as the plural and your class as the singular.
 *  So if you have a table named "widgets", your class should be named "widget".
 *  This is not a hard rule and can be bent when your table or class names might be different
 *  due to namespace conflicts.
 *
 *  You can easily set fields by $widget->field = $value;  The setting and getting of a field
 *  is validated against your schema.
 */


/**
 * This is a very simple example.  The name of your table is 'widgets'.
 */

roots_include('record');
class Widget extends RootsRecord {
  protected function table() {
    return 'widgets';
  }
}

/**
 * If your table has a serialized field you can tell Roots_Record to look for it.
 *
 * This looks for the serialzed fields in your schema and unserilzes them when the record is loaded.
 */
roots_include('record');
class Widget extends RootsRecord {
  protected $_should_unserialize = TRUE;

  protected function table() {
    return 'widgets';
  }
}

/**
 * You can also specify validation
 */
roots_include('record');
class Widget extends RootsRecord {
  protected function table() {
    return 'widgets';
  }

  protected function validate() {
    if (empty($this->name)) {
      return FALSE;
    }

    return TRUE:
  }
}

/**
 * You can use the Roots_Find class to search for content.
 *
 *  The Roots_Find class has 2 public static functions.  findy_by_string and find_by_number.
 *  Both take the same arugments.  THe name of the field, the value to find it by and an instanse of the class.
 *  Unfortunatily, the oject is requred in php 5.2 due to the inability to do late static binding.
 *
 *  Best practice is to name your function find_by_<field>.  Any field should also have an index against it.
 */
roots_include('record');
class Widget extends RootsRecord {
  protected function table() {
    return 'widgets';
  }

  /**
   * This is an example of finding a widget by an id
   *
   * @param <int> $id
   * @return Widget object.
   */
  public static function findByID($id) {
    $widget = new Widget();
    return array_pop(Rootsfind::findByNumber('id', $id, $widget));
  }

  /**
   * This is an example of finding a widget by the types.
   *
   * @param <string> Type
   * @return <array> Array of widget objects
   */
  public static function findByType($type) {
    $widget = new Widget();
    return Rootsfind::findByString('type', $type, $widget);
  }
}

/**
 * Simple example of using this class
 */
function saving_a_widget() {
  $widget = new Widget;
  // This represents the 'type' field on your table.
  $widget->type = 'Simple';
  // THis represents the name field on the table.
  $widget->name = 'Cool Widget';
  // This do nothing since id is always serial
  $widget->id = 123;
  // The invalid_field field doesn't exist on your table so it will do nothing.
  $widget->invalidField = 'foo';
  // This saves the record to the database.
  if (!$widget->save()) {
    // You can access any errors
    $errors = $widget->getErrors();
  }
}

/**
 * Loading a widget
 */
function loading_a_widget() {
  // You can load a widget by an id if you know it
  $widget = new Widget($id);

  // You can pre populate the fields on the widget
  $fields = array(
    'type' => 'simple',
    'name' => 'Cool Widget',
  );
  $widget - new Widget($fields);

  // You can also create a widget and save it to the db in one step
  // Pass in an array of your fields and the name of the class to return.
  $widget = RootsRecord::create($fields, 'widget');
}

/**
 * Accessing a widget
 */
function access_a_widget() {
  // You can get an array of the field values
  $array_of_fields = $widget->toArray();

  // Use the static function to get a single object
  $widget = Widget::findById($id);

  // Get an array of the Widget objects that are of simple type.
  $simple_widgets = Widget::findByType('simple');
}
