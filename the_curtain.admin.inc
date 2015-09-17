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

function _the_curtain_admin_variable() {
  return _the_curtain_admin_theme_table('variable', array('name'));
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

function _the_curtain_watchdog_stats() {

  $results = db_query('select type, message, variables, COUNT(*) AS count FROM watchdog w
    GROUP BY type, message, variables HAVING count > 1 ORDER BY count DESC');

  while ( $result = $results->fetchAssoc() ) {
    // create the column headers from the field names
    if ( ! isset($header) ) {
      $header = array();
      foreach ( (array)$result as $field => $value ) {
        if ( ! isset($header[$field]) ) {
          $header[$field] = $field;
        }
      }
    }

    // turn the row into an array for easy drupal formatting.
    $rows[] = (array)$result;

    dsm($result);
  }

  return theme('table', array('header'=>$header,'rows'=>$rows));
}

function _the_curtain_admin_theme_table($table, array $orderBy = NULL) {

  $query = db_select($table, 't')->fields('t')
    ->addTag(__FUNCTION__);
  foreach ( $orderBy as $field ) {
    $query->orderBy($field);
  }
  $results = $query->execute()->fetchAll();

  foreach ( $results as $result ) {

    // create the column headers from the field names
    if ( ! isset($header) ) {
      $header = array();
      foreach ( (array)$result as $field => $value ) {
        if ( ! isset($header[$field]) ) {
          $header[$field] = $field;
        }
      }
    }

    // turn the row into an array for easy drupal formatting.
    $result = (array)$result;

    // unserialize serialized values
    foreach ( $result as &$value) {
      if ( $unserialized = @unserialize($value) ) {
        if ( is_object($unserialized) ) {
          $unserialized = (array)$unserialized;
        }
        elseif ( is_array($unserialized) ) {
          $vals = array();
          foreach ( $unserialized as $key => $val ) {
            if ( is_object($val) ) {
              $val = (array)$val;
            }
            $vals[] = array($key, $val);
          }
          $value = theme('table', array('rows'=>$vals));
        }
        else {
          $value = $unserialized;
        }
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
