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
  $results = db_select($table, 't')->fields('t')
    ->execute()->fetchAll();
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

function _the_curtain_admin_unveilings_form($form, &$form_state) {
  $unveilings = array(
    'the_curtain_show_sql' => t('Show SQL'),
    'the_curtain_show_forms' => t('Show Form Arrays'),
    'the_curtain_show_form_states' => t('Show Form State Arrays'),
  );
  foreach ( $unveilings as $unveiling => $description) {
    $form[$unveiling] = array(
      '#title' => $description,
      '#type' => 'checkbox',
      '#default_value' => variable_get($unveiling),
    );
  }
  return system_settings_form($form);
}
