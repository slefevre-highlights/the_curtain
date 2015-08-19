<?php
/**
 * @file administrative page building functions
 */

/**
 * Page callback
 */
function _the_curtain_admin_page() {
  return t('Get your data straight from the horses mouth&mdash; the database.');
}

function _the_curtain_admin_aliases() {
  return _the_curtain_admin_theme_table('url_alias');
}
function _the_curtain_admin_menu_router() {
  return _the_curtain_admin_theme_table('menu_router');
}
function _the_curtain_admin_system() {
  return _the_curtain_admin_theme_table('system');
}
function _the_curtain_admin_cache_bootstrap() {
  return _the_curtain_admin_theme_table('cache_bootstrap');
}

function _the_curtain_admin_hooks() {
#  db_query('SELECT
}

function _the_curtain_admin_theme_table($table) {
  $results = db_select($table, 't')->fields('t')
    ->addTag('_the_curtain_theme_table')
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
    $result = (array)$result;
    // unserialize serialized arrays or objects
    foreach ( $result as &$value) {
      if ( $test = @unserialize($value) ) {
        $value = '<pre>' . print_r($test, TRUE) . '</pre>';
      }
    }
    $rows[] = (array)$result;
  }
  return theme('table', array('header'=>$header,'rows'=>$rows));
}

function _the_curtain_admin_tables_form($form, &$form_state) {
  $tables = db_query('SHOW TABLES');

  while ( $table = $tables->fetchAssoc() ) {
    foreach ( $table as $key => $value ) {
      $options[$value] = $value;
    }
  }

  $form['table'] = array(
    '#type' => 'select',
    '#title' => t('Tables'),
    '#options' => $options,
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Submit'),
    '#submit' => array('_the_curtain_admin_show_tables_submit'),
  );
  return $form;
}

function _the_curtain_admin_show_tables_submit($form, &$form_state) {
  $form_state['redirect'] = 'admin/config/development/the_curtain/tables/'
    . check_plain($form_state['values']['table']);
}

function _the_curtain_admin_unveilings_form($form, &$form_state) {
  $unveilings = array(
    'the_curtain_show_sql' => t('Show SQL'),
    'the_curtain_show_forms' => t('Show Form Arrays'),
    'the_curtain_show_form_states' => t('Show Form State Arrays'),
    'the_curtain_show_efq' => t('Show Entity Field Queries SQL'),
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
