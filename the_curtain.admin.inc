<?php
/**
 * @file administrative page building functions
 */

/**
 * Page callback
 */
function _the_curtain_admin_page() {
  return 'blank.';
}

function _the_curtain_admin_aliases() {
  $results = db_query('SELECT pid, source, alias, language FROM url_alias')->fetchAll();
  return _the_curtain_admin_theme_table($results);
}
function _the_curtain_admin_menu_router() {
  $results = db_query('SELECT * FROM menu_router')->fetchAll();
  return _the_curtain_admin_theme_table($results);
}
function _the_curtain_admin_system() {
  $results = db_query('SELECT * FROM system')->fetchAll();
  return _the_curtain_admin_theme_table($results);
}
function _the_curtain_admin_modules() {
  $results = db_query('SELECT * FROM cache_bootstrap')->fetchAll();
  return _the_curtain_admin_theme_table($results);
}

function _the_curtain_admin_theme_table($results) {
  foreach ( $results as $result ) {
    if ( ! isset($header) ) {
      $header = array();
      foreach ( (array)$result as $field => $value ) {
        if ( ! isset($header[$field]) ) {
          $header[$field] = t($field);
        }
      }
    }
    $rows[] = (array)$result;
  }
  return theme('table', array('header'=>$header,'rows'=>$rows));
}
