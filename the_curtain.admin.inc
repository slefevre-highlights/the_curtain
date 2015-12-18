<?php
/**
 * @file administrative page building functions
 */

/**
 * Page callback
 */
function _the_curtain_admin_page() {
  return t('All shall be revealed.');
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
  return "Under Construction";
}

function _the_curtain_watchdog_stats() {

  $results = db_query('select type, message, variables, COUNT(*) AS count FROM watchdog w
    GROUP BY type, message, variables HAVING count > 4 ORDER BY count DESC');

  $header = array('Type', 'Count', 'Message');
  $rows = array();

  foreach ( $results as $result ) {
    // t() message and variables
    $result = (array)$result;
    $result['message'] = t($result['message'], unserialize($result['variables']));
    unset($result['variables']);
    $rows[] = array(
      $result['type'],
      $result['count'],
      $result['message'],
    );
  }

  return theme('table', array('header'=>$header,'rows'=>$rows));
}

function _the_curtain_admin_theme_table($table, array $orderBy = NULL, $limit = 100) {

  $query = db_select($table, 't')->fields('t')
    ->addTag(__FUNCTION__);
  foreach ( $orderBy as $field ) {
    $query->orderBy($field);
  }
  $query->range(0,$limit);
  $results = $query->execute()->fetchAll();

  foreach ( $results as $result ) {

    // create the column headers from the field names
    if ( ! isset($header) ) {
      $header = array();
      foreach ( (array)$result as $field => $value ) {
        if ( ! isset($header[$field]) ) {
          $header[$field] = ucwords($field);
        }
      }
    }

    // turn the row into an array for easy drupal formatting.
    $result = (array)$result;

    foreach ( $result as &$value) {
      if ( strpos($value, ':') === 1 ) {
        $value = _the_curtain_test_serialization($value);
      }
      elseif ( $value == 'N;' ) {
        // NULL
        $value = '(literal) NULL';
      }
    }
    $rows[] = (array)$result;
  }
  return theme('table', array('header'=>$header,'rows'=>$rows));
}

function _the_curtain_test_serialization($value) {
  // unserialize serialized values

  if ( $value == 'a:0:{}' ) {
    $value = "(serialized empty array)" . theme('table', array('rows'=>array(array("&nbsp;"))));
  }
  elseif ( $value == 'b:0;' ) {
    $value = "(serialized boolean) FALSE";
  }
  elseif ( $value == 'b:1;' ) {
    $value = "(serialized boolean) TRUE";
  }
  elseif ( $value == 's:0:"";' ) {
    $value = '(serialized empty string) ""';
  }
  elseif ( strpos($value, 's:O:') === 0 ) {
     // object
    $value = '(serialized object)' . (array)unserialize($value);
  }

  /*
  // this is acting problematic
  elseif ( strpos($value, 'a:') === 0 ) {
    $vals = array();
    foreach ( @unserialize($value) as $key => $val ) {
      if ( is_object($val) ) {
        $val = (array)$val;
      }
      $vals[] = array($key, $val);
    }
    $value = '(serialized array)' . theme('table', array('rows'=>$vals));
  }

  */

  elseif (
       strpos($value, 'i:') === 0
    || strpos($value, 's:') === 0
    || strpos($value, 'd:') === 0
  ) {
    $value = @unserialize($value);
    if ( is_string($value) && is_numeric($value) ) {
      $value = '(serialized string numeral) "' . $value . '"';
    }
  }

  return $value;
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

function _the_curtain_admin_fields() {

  $field_map = field_info_field_map();
  foreach ( $field_map as $field => $info ) {
    foreach( $info['bundles'] as $bundle => $appearances ) {
      $bundle_rows = array();
      foreach ( $appearances as $appearance ) {
        $bundle_rows[] = array($appearance);
      }
      $bundle_info = theme('table', array(
        'header' => array($bundle),
        'rows' => $bundle_rows,
      ));
    }
    $rows[] = array($field, $info['type'],$bundle_info );
  }
  kpr(field_info_field_map());

  return theme('table',array(
    'header' => array(t('Name'),t('Type'),t('Bundles')),
    'rows'=> $rows,
  ));
}
