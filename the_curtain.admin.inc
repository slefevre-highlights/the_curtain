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
  return _the_curtain_admin_theme_table('aliases');
}
function _the_curtain_admin_menu_router() {
  return _the_curtain_admin_theme_table('menu_router');
}
function _the_curtain_admin_system() {
  return _the_curtain_admin_theme_table('system');
}
function _the_curtain_admin_modules() {
#  return _the_curtain_admin_theme_table('modules');
}

function _the_curtain_admin_theme_table($table) {
  $results = db_query("SELECT * FROM {" . $table . "}")->fetchAll();
  foreach ( $results as $result ) {
    if ( ! isset($header) ) {
      $header = array();
      foreach ( (array)$result as $field => $value ) {
        if ( ! isset($header[$field]) ) {
          $header[$field] = $field;
        }
      }
    }
    $rows[] = (array)$result;
  }
  return theme('table', array('header'=>$header,'rows'=>$rows));
}
