<?php
/**
 * @file administrative page building functions
 */

/**
 * Page callback
 */
function _the_curtain_admin() {
  return 'blank.';
}

function _the_curtain_admin_aliases() {
  $results = db_query('SELECT pid, source, alias, language FROM url_alias');
  return _the_curtain_admin_aliases($results); 
}

function _the_curtain_admin_theme_table($results) {
  foreach ( $results as $field => $values ) {
    if ( ! isset($header) ) {
      $header = array();
    }
    if ( ! isset($header[$field]) ) {
      $header[$field] = t($field);
    }
    $rows[] = $results; 
  }

  return theme('table', array('header'=>$header,'rows'=>$rows));
}
