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

function _the_curtain_admin_display_table($table) {
  $data = _the_curtain_admin_entable_table($table);
  return _the_curtain_admin_theme_table($data['header'], $data['rows']);
}

function _the_curtain_admin_hooks() {

  // load all module files
  $module_list = system_list('module_enabled');

  foreach( $module_list as $module_info ) {
    $module_path = substr($module_info->filename, 0,  strrpos($module_info->filename, "/")  ) ;
  }

  // use `module_implements()`
  $functions = get_defined_functions();
  $user_funcs = $functions['user'];
  foreach ( $user_funcs as $func ) {
    if ( stristr($func, 'hook') ) {
      $hooks[] = array($func);
    }
  }

  return theme('table',array('header'=>array('Hook'),'rows'=>$hooks));
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

function _the_curtain_admin_entable_table($table, array $orderBy = NULL, $limit = 100) {

  $query = db_select($table, 't')->fields('t')
    ->addTag(__FUNCTION__);
  foreach ( $orderBy as $field ) {
    $query->orderBy($field);
  }

  // Query options

  // limit & offset
  if ( isset($_GET['limit']) && is_numeric($_GET['limit']) ) {
    $limit = $_GET['limit'];
  }

  $offset = 0;
  if ( isset($_GET['offset']) && is_numeric($_GET['offset']) ) {
    $offset = $_GET['offset'];
  }

  $query->range($offset,$limit);

  // order by
  if ( isset($_GET['order_by']) ) {
     foreach ( explode($_GET['order_by']) as $sort_field ) {
       $query->orderby($sort_field);
     }
  }

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

  return array('header'=>$header,'rows'=>$rows);
}

function _the_curtain_admin_theme_table($header, $rows) {
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

function _the_curtain_admin_table($table) {
  $data = _the_curtain_admin_entable_table($table);
  return _the_curtain_admin_theme_table($data['header'], $data['rows']);
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
    'the_curtain_show_form_id' => t('Show Form IDs (names)'),
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

function _the_curtain_admin_views() {

  // find field usage in views
  $views_fields = array();
  $results = db_query('SELECT views_display.*, views_view.* FROM views_display
  INNER JOIN views_view ON views_display.vid = views_view.vid');

  $rows = array();
  foreach ( $results as $result ) {

    if ( ! isset($headers) ) {
      foreach ( (array)$result as $field => $value ) {
        if ( $field != 'display_options' ) {
          $headers[] = $field;
        }
      }
    }

    // load all the row values ...
    $rows[$result->vid] = (array)$result;

    // ... unset the serialized array ...
    unset($rows[$result->vid]['display_options']);

    // ... and process it as if it were columns
    $data = unserialize($result->display_options);

    foreach ( $data as $key => $value ) {

      // any display options field could potentially add new keys
      foreach ( $data as $key => $value ) {
        $headers[$key] = $key;
      }

      $rows[$result->vid][$key] = $value;
    }
  }

  return theme('table',array(
    'header' => $headers,
    'rows'=> $rows,
  ));
}

/**
 * Show all things that can be a path on the site.
 */
function _the_curtain_admin_paths() {

  $output = _the_curtain_admin_display_table('url_alias');

  if ( module_exists('path') ) {
    $output .= _the_curtain_admin_display_table('url_alias');
  }

  // views paths?

  return $output;
}

function _the_curtain_admin_fields() {

  // find field usage in views
  $views_fields = array();
  $results = db_query('SELECT display_options, display_title, name, human_name FROM views_display
  INNER JOIN views_view ON views_display.vid = views_view.vid');
  foreach ( $results as $result ) {
    $data = unserialize($result->display_options);
    if ( isset($data['fields']) ) {
      foreach ( $data['fields'] as $field => $field_options ) {
         $views_fields[$field][$result->human_name][] = $result->display_title;
      }
    }
  }

  // get field info from drupal
  $field_map = field_info_field_map();
  ksort($field_map);
  foreach ( $field_map as $field => $info ) {
    $bundle_info = "";
    foreach( $info['bundles'] as $bundle => $appearances ) {
      $bundle_rows = array();
      foreach ( $appearances as $appearance ) {
        $bundle_rows[] = array($appearance);
      }
      $bundle_info .= theme('table', array(
        'header' => array($bundle),
        'rows' => $bundle_rows,
      ));

      // tack on views field information.
      if ( isset($views_fields[$field])) {
        $views_rows = array();
        foreach ( $views_fields[$field] as $view_name => $displays ) {
          foreach ( $displays as $display ) {
            $views_rows[] = array("$view_name: $display");
          }
        }
        $views_info = theme('table', array(
          'header' => array('views displays'),
          'rows' => $views_rows,
        ));
        $bundle_info .= $views_info;
      }
    }
    $rows[] = array($field, $info['type'],$bundle_info );
  }

  return theme('table',array(
    'header' => array(t('Name'),t('Type'),t('Bundles')),
    'rows'=> $rows,
  ));
}
