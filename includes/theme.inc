<?php

/**
 * @file
 * Roots Theme Functions
 */

/**
 * Wrap content in an HTML element, with optional class and ID
 *
 * @param $content what to place within the element
 * @param $element The type of element
 * @param $style What to add to the 'class' part of the element
 * @param $id the id of the element
 *
 * @return formted HTML element
 */
function theme_html_element($content = NULL, $element = 'div', $style = NULL, $id = NULL) {
  if (!$content) {
    return;
  }
  $style = $style ? " class=\"$style\"" : '';
  $id = $id ? " id=\"$id\"" : '';
  return "<$element$style$id>$content</$element>\n";
}// end - function