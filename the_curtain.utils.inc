<?php
/**
 * @file
 * Auxiliary functions
 */

/**
 * Translates a select query interface to an sql string
 * http://drupal.stackexchange.com/a/51338/1441
 */
function the_curtain_query_to_sql($query) {
  $string = (string) $query;
  $arguments = $query->arguments();
  if (is_array($arguments)) {
    foreach ($arguments as $placeholder => &$value) {
      if (is_string($value)) {
        $value = "'$value'";
      }
    }
    $string = strtr($string, $arguments);
  }
  return ($string);
}
