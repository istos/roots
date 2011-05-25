<?php

/**
 * @file Taxonomy Based Functions
 *
 * @defgroup roots_taxonomy Taxonomy Functions
 * @{
 * Taxonomy Based FUnctions.
 */

/**
 * Get the term objects for a node
 * @param $node nid or node object
 * @param $vocab vid or vocab name
 * @param $field field of the term obejct to retern
 *  retern array is formated at array(tid => field)
 *
 * @return array of term objects
 */
function roots_get_node_terms($node, $vocab = 'Site Section', $field = NULL) {
  if (is_numeric($node)) {
    $node = node_load($node);
  }

  if (is_string($vocab)) {
    $vocab = roots_get_vocab_vid($vocab);
  }

  // Get the term id's
  $output = array();
  if (is_array($node->taxonomy)) {
    foreach ($node->taxonomy as $key => $terms) {
      if (is_array($terms) && $key == $vocab) {
        foreach ($terms as $tid) {
          if (is_numeric($tid)) {
            $term2 = taxonomy_get_term($tid);
          }
          $terms[$tid] = $term2;
        }
      } elseif (is_numeric($terms)) {
        $term2 = taxonomy_get_term($terms);
        if ($term2->vid !== $vocab) {
          continue;
        }
      } elseif (is_object($terms) && $terms->vid == $vocab) {
        $term2 = $terms;
      } else {
        continue;
      }

      if (isset($field) && isset($term2->$field)) {
        $output[$term2->tid] = $term2->$field;
      } else {
        $output[$term2->tid] = $term2;
      }
    }
  }
  return $output;
}

/**
 * helper function to get any free tags into a more useful form
 * strips out empty vocabularies, splits the terms into an array per vid
 *
 * @param $node - the node object or nid
 * @return $free_tag_vocabs - array($vid => array(0 => 'term1', 1 => 'term2',))
 */
function roots_get_free_tags($node) {
  if (is_numeric($node)) {
    $node = node_load($node);
  }

  $free_tag_vocabs = array();
	foreach ($node->taxonomy as $vocabulary_id => $term) {
    if ($vocabulary_id == 'tags') {
      foreach ($term as $tags_vid => $tag) {
        if (strlen($tag)) {
          $free_tag_vocabs[$tags_vid] = split(', ', $tag);
        }
      }
    }
  }
  return $free_tag_vocabs;
}

/**
 * Get the Vocab ID for a vocab
 *
 * @param $vocab_name string value of the vocab name
 * @param $slave bool If using pressflow, if we should look for the slave mysql instance
*/
function roots_get_vocab_vid($vocab_name, $slave = TRUE) {
  // Cache.  Less db hits the better
  static $output = array();

  if (!$output[$vocab_name]) {
    // Use db_query_slave if we are using pressflow
    if (function_exists('db_query_slave') && $slave) {
      $output[$vocab_name] = db_result(db_query_slave("SELECT vid FROM {vocabulary} WHERE name = '%s'", $vocab_name));
    } else {
      $output[$vocab_name] = db_result(db_query("SELECT vid FROM {vocabulary} WHERE name = '%s'", $vocab_name));
    }
  }

  return $output[$vocab_name];
}

/**
 * Return a nested array of taxonomy terms.
 *
 * Terms in an array that you can access with the $term->children.
 *
 * @param $terms int/array vocabulary ID or array of terms from taxonomy_get_tree().  
 *   Use with roots_get_vocab_vid().
 * @param $max_depth int Depth Limit.
 * @param $parent int parent tid (mainly for internal use).
 * @param $parents_index internal use only.
 * @param $depth internal use only.
 *
 * @return Nexted array of term objects.
 * - term object.
 * - children: array of child term objects with the same children array.
 */
function roots_taxonomy_get_nested_tree($terms = array(), $max_depth = NULL, $parent = 0, $parents_index = array(), $depth = 0) {
  if (is_int($terms)) {
    $terms = taxonomy_get_tree($terms);
  }

  foreach($terms as $term) {
    foreach($term->parents as $term_parent) {
      if ($term_parent == $parent) {
        $return[$term->tid] = $term;
      }
      else {
        $parents_index[$term_parent][$term->tid] = $term;
      }
    }
  }

  foreach($return as &$term) {
    if (isset($parents_index[$term->tid]) && (is_null($max_depth) || $depth < $max_depth)) {
      $term->children = taxonomy_get_nested_tree($parents_index[$term->tid], $max_depth, $term->tid, $parents_index, $depth + 1);
    }
  }

  return $return;
}

/**
 * @} End Taxonomy Group
 */