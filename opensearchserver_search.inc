<?php
/**
 * @file
 * Search fields
 */

/**
 * Implements hook_theme().
 */
function opensearchserver_theme($existing, $type, $theme, $path) {
  return array(
    'opensearchserver_search' => array(
      'arguments' => array('opensearchserver_data' => NULL),
      'template' => 'opensearchserver',
    )
  );
}

function opensearchserver_preprocess_search_theme_form(&$vars, $hook) {
  $vars['form']['search_theme_form']['#value'] = t('Search this Site');
}

/**
 * Implements hook_form_alter().
 */
function opensearchserver_form_alter(&$form, $form_state, $form_id) {
  $form_id_processed = $form_id;
  $value = isset($arg[2]) ? $arg[2] : '';
  switch ($form_id_processed) {
    case 'search_form':
      $form['basic']['inline']['keys'] = array(
        '#type' => 'textfield',
        '#title' => '',
        '#default_value'  => $value,
        '#maxlength' => 255,
      );
      $form['basic']['inline']['submit'] = array(
        '#type' => 'submit',
        '#value' => t('Search'),
      );
    $form['#submit'][] = 'opensearchserver_submit';
    break;
    case 'search_theme_form':
      $form['search_theme_form'] = array(
        '#type' => 'textfield',
        '#title' => '',
        '#size' => 15,
        '#maxlength' => 255,
      );
      $form['submit'] = array(
        '#type' => 'submit',
        '#value' => t('Search'),
      );
      $form['#submit'][] = 'opensearchserver_submit';
    break;
    case 'search_block_form':
      $form['search_block_form'] = array(
        '#type' => 'textfield',
        '#title' => '',
        '#size' => 15,
        '#maxlength' => 255,
      );
      $form['submit'] = array(
        '#type' => 'submit',
        '#value' => t('Search'),
      );
      $form['#submit'][] = 'opensearchserver_submit';
    break;
  }
  unset($form['#token']);
}

/**
 * Get the language prefix .
 */

function get_language_prefix() {
  global $language , $base_root;
  if ( $language->prefix ) {
    $url = 'q='. $language->prefix . '/opensearchserver/search/';
  }
  else {
    $url = 'q=opensearchserver/search/';
  }
  return $url;
}
/**
 * Implements form submission.
 */

function opensearchserver_submit($form, &$form_state) {
  global $language , $base_root;
  $lang_name = $language->language;
  $url = get_language_prefix();
  if ($form_state['values']['search_theme_form']) {
    drupal_goto($base_root , $url . check_plain($form_state['values']['search_theme_form']) . '&lq=' . $lang_name);
  }
  elseif ($form_state['values']['search_block_form']) {
    drupal_goto($base_root , $url . check_plain($form_state['values']['search_block_form']) . '&lq=' . $lang_name);
  }
  else {
    drupal_goto($base_root, $url . check_plain($form_state['values']['keys']) . '&lq=' . $lang_name);
  }
}
/**
 * View an search result in opensearchserver.tpl.php
 */
function opensearchserver_search() {
  global $base_url;
  $arg = arg(NULL, $_GET['q']);
  $block_tool = db_query("
    SELECT
    noresult_filter_enable,
    block_text,
    filter_enabled,
    url_snippet,
    date_filter,
    signature,
    enable_language_filter
    FROM
    {opensearchserver}"
  );
  $search_settings=db_fetch_object($block_tool);
  $tq = isset($_REQUEST['tq']) ? $_REQUEST['tq'] : NULL;
  $fq = isset($_REQUEST['fq']) ? $_REQUEST['fq'] : NULL;
  $ts = isset($_REQUEST['ts']) ? $_REQUEST['ts'] : NULL;
  $lq = isset($_REQUEST['lq']) ? $_REQUEST['lq'] : NULL;
  $opensearchserver_data['q']=$arg[2];
  $result=opensearchserver_get_search_result($arg[2], TRUE);
  $result_facet=opensearchserver_get_search_result($arg[2], FALSE);
  $opensearchserver_data['paging']=opensearchserver_paging($result);
  $timestamp_array = array("past24" => "Past 24 hours", "pastweek" => "Past week", "pastmonth" => "Past month", "pastyear" => "Past year");
  $opensearchserver_data['tq'] = $tq;
  $opensearchserver_data['fq'] = $fq;
  $opensearchserver_data['ts'] = $ts;
  $opensearchserver_data['lq'] = $lq;
  $opensearchserver_data['result'] = $result;
  $opensearchserver_data['result_facet'] = $result_facet;
  $opensearchserver_data['base_url'] = $base_url;
  $opensearchserver_data['block_text'] = $search_settings->block_text;
  $opensearchserver_data['no_filter'] = $search_settings->noresult_filter_enable;
  $opensearchserver_data['filter_enabled'] = $search_settings->filter_enabled;
  $opensearchserver_data['date_filter'] = $search_settings->date_filter;
  $opensearchserver_data['signature'] = $search_settings->signature;
  $opensearchserver_data['url_snippet'] = $search_settings->url_snippet;
  $opensearchserver_data['form'] = drupal_get_form('opensearchserver_page_form');
  $opensearchserver_data['time_stamp'] = $timestamp_array;
  $opensearchserver_data['enable_language_filter'] = $search_settings->enable_language_filter;
  $opensearchserver_data['oss_result'] = get_search_result($result);
  $opensearchserver_data['oss_result_facet'] = get_search_result($result_facet);
  $opensearchserver_data['result_time'] = get_result_time($opensearchserver_data['result']);
  $opensearchserver_data['filter_result'] = get_filter_result($opensearchserver_data['oss_result'], $opensearchserver_data['filter_enabled'], $opensearchserver_data['block_text'] , $opensearchserver_data['no_filter']);
  $opensearchserver_data['facet_everything'] = get_facet_everything($opensearchserver_data['fq'], $opensearchserver_data['tq'], $opensearchserver_data['ts'], $opensearchserver_data['oss_result_facet'], $opensearchserver_data['oss_result']);
  $opensearchserver_data['print_facet_everything'] = generate_facet_everything($opensearchserver_data['facet_everything'], $opensearchserver_data['q']);
  return theme('opensearchserver_search', $opensearchserver_data);
}


/**
 * Get search result object
 */
function get_search_result($result) {
  return new OssResults($result, FALSE);
}

/**
 * Paging for SearchResults.
 */

function opensearchserver_paging($result) {
  if ($result != NULL) {
  $oss_paging = new OssPaging($result, 'r', 'p');
  $paging_array = array();
  if (isset($oss_paging) && $oss_paging->getResultTotal() >= 1) {
    if ($oss_paging->getResultLow() > 0) {
      $style='oss_pager_first';
      $label = 'First';
      $url = $oss_paging->getPageBaseURI() . '1';
      $paging_array[] = array('style' => $style, 'label' => $label, 'url' => $url);
    }
    if ($oss_paging->getResultPrev() < $oss_paging->getResultCurrentPage()) {
      $style='oss_pager_prev';
      $label = 'Previous';
      $url = $oss_paging->getPageBaseURI() . ($oss_paging->getResultPrev() + 1);
      $paging_array[] = array('style' => $style, 'label' => $label, 'url' => $url);
    }
    for ($i = $oss_paging->getResultLow(); $i < $oss_paging->getResultHigh(); $i++) {
      $style='oss_pager_number';
      if ($i == $oss_paging->getResultCurrentPage()) {
        $label = $i + 1;
        $url = NULL;
      }
      else {
        $label = $i + 1;
        $url = $oss_paging->getPageBaseURI() . $label;
      }
      $paging_array[] = array('style' => $style, 'label' => $label, 'url' => $url);
    }
    if ($oss_paging->getResultNext() > $oss_paging->getResultCurrentPage()) {
      $style='oss_pager_next';
      $label = 'Next';
      $url = $oss_paging->getPageBaseURI() . ($oss_paging->getResultNext() + 1);
      $paging_array[] = array('style' => $style, 'label' => $label, 'url' => $url);
    }
  }
  return $paging_array;
  }
}

function opensearchserver_create_url_snippet($url, $end) {
  if (drupal_strlen($url) > $end && $end) {
    $snippet_url = drupal_substr($url, 0, $end);
    return check_url($snippet_url) . '..';
  }
  else {
    return check_url($url);
  }
}

function get_max($oss_result) {
  return ($oss_result->getResultStart() + $oss_result->getResultRows() > $oss_result->getResultFound()) ? $oss_result->getResultFound() : $oss_result->getResultStart() + $oss_result->getResultRows();
}
/**
 * Get the search result from the index.
 */
function get_result_time($result) {
  return (float)$result->result['time'] / 1000;
}
/**
 * Get the search result from the index.
 */

function opensearchserver_get_search_result($query, $facet) {
  if ($query) {
    global $user;
    $get_details = db_query("
      SELECT `serverurl`,
      `indexname`,`username`,
      `key`,`log_enable`,`enable_language_filter`
      FROM {opensearchserver}"
    );
    $server_details = db_fetch_object($get_details);
    $start = isset($_REQUEST['p']) ? $_REQUEST['p'] : NULL;
    $start = isset($start) ? max(0, $start - 1) * 10 : 0;
    $escapechars = array('\\', '^', '~', ':', '(', ')', '{', '}', '[', ']' , '&&', '||', '!', '*', '?');
    foreach ($escapechars as $escchar) {
      $query = str_replace($escchar, ' ', $query);
    }
    $query = trim($query);
    $search = new OssSearch($server_details->serverurl, $server_details->indexname, 10, $start);
    $search->credential($server_details->username, $server_details->key);
    if ($server_details->log_enable) {
      $search->setLog(TRUE);
      $search->setCustomLog(1, session_id());
      if ( $user->uid ) {
        $search->setCustomLog (2, $user->name);
      }
      else {
        $search->setCustomLog (2, "Guest");
      }
      $search->setCustomLog (3, $_SERVER['REMOTE_ADDR']);
    }
    $search->facet('type', 1);
    $search->facet('taxonomy', 1);
    $search->facet('lang', 1);
    if ($facet) {
      $filter=$_REQUEST['fq'];
      if ($filter && $filter != 'All') {
        $search->filter('type:"' . $filter . '"');
      }
      if ($server_details->enable_language_filter) {
        $lang_filter=$_REQUEST['lq'];
        if ($lang_filter && $lang_filter != 'All') {
          $search->filter('lang:"' . $lang_filter . '"');
        }
      }
      $filter_categories=$_REQUEST['tq'];
      if ($filter_categories && $filter_categories != 'All') {
        $search->filter('taxonomy:"' . $filter_categories . '"');
      }
      $timestamp_filter=$_REQUEST['ts'];
      if ($timestamp_filter && $timestamp_filter != 'All') {
        if ( $timestamp_filter=="Past 24 hours") {
          $search->filter('content_timestamp:[' . date("YmdHis", mktime(0, 0, 0, date("m"), date("d")-1, date("Y"))) . ' TO ' . date("YmdHis") . ']');
        }
        elseif ($timestamp_filter=="Past week") {
          $search->filter('content_timestamp:[' . date("YmdHis", mktime(0, 0, 0, date("m"), date("d")-7, date("Y"))) . ' TO ' . date("YmdHis") . ']');
        }
        elseif ($timestamp_filter=="Past month") {
          $search->filter('content_timestamp:[' . date("YmdHis", mktime(0, 0, 0, date("m"), date("d")-31, date("Y"))) . ' TO ' . date("YmdHis") . ']');
        }
        elseif ($timestamp_filter=="Past year") {
          $search->filter('content_timestamp:[' . date("YmdHis", mktime(0, 0, 0, date("m"), date("d")-365, date("Y"))) . ' TO ' . date("YmdHis") . ']');
        }
      }

    }
    $result = $search->query($query)->template('search')->execute(5);
  }
  return $result;
}

/**
 * Implements drupal search form submit.
 */

function opensearchserver_page_form_submit($form_id, &$form_state) {
  global $language, $base_url;
  $lang_name = $language->language;
  $url = get_language_prefix();
  drupal_goto($base_url, $url . check_plain($form_state['values']['q']) . '&lq=' . $lang_name);
}

function generate_facet_everything($facet_everything, $query) {
  if ($facet_everything) {
    $output = '<li><b>'. generate_url_everything($query) .'</b></li>';
  }
  else {
    $output = '<li>'. generate_url_everything($query) .'</li>';
  }
  return $output;
}

/**
 * Generate url for everything in facet
 */

function generate_url_everything($query) {
  $lq = isset($_REQUEST['lq']) ? $_REQUEST['lq'] : NULL;
  $url = '?' . get_language_prefix() . drupal_urlencode($query) . '&lq=' . $lq;
  return '<a id="oss_facet" href="'. $url .'"> Everything</a>';
}

/**
 * Check the filter result is available or not
 */

function get_filter_result($oss_result, $filter_enabled , $block_text, $no_filter) {
  $check_filter_enabled = $filter_enabled == 1 && $search_query != $block_text;
  $check_filter_at_zero_result = $oss_result->getResultFound() <= 0 && $no_filter == 1;
  if ($check_filter_enabled && $check_filter_at_zero_result) {
    $filter_result = TRUE;
  }
  elseif ($check_filter_enabled && $oss_result->getResultFound() > 0) {
    $filter_result = TRUE;
  }
  else {
    $filter_result = FALSE;
  }
  return  $filter_result;
}

/**
 * Generate the filter for everything.
 */

function get_facet_everything($facet_fq, $facet_tq, $facet_ts, $oss_result_facet, $oss_result) {
  $check_facet_available = $facet_fq == NULL && $facet_tq == NULL && $facet_ts == NULL &&  $oss_result_facet >= 0 ;
  $check_facet_available_atzero_result = $oss_result->getResultFound() <= 0 && $oss_result_facet <= 0;
  return ($check_facet_available || $check_facet_available_atzero_result) ? TRUE :FALSE ;
}
/**
 * Get the native name of the language.
 */
function get_local_facet($lang) {
  $url_prefix = db_query("
      SELECT language,
      native,name
      FROM
      {languages}
      WHERE
      languages.language='%s' AND languages.enabled='%d'", $lang , '1'
  );
  $url_prefix_object = db_fetch_object($url_prefix);
  return $url_prefix_object->native;
}
/**
 * Generate the filter for link
 */
function generate_filter_link($q, $facet_name , $facet_value) {
  $base = '?' . get_language_prefix();
  $lq = isset($_REQUEST['lq']) ? $_REQUEST['lq'] : NULL;
  if ($facet_name != '&lq=') {
  return $base . drupal_urlencode($q) . $facet_name . drupal_urlencode($facet_value) . '&lq=' . $lq;
  }
  else {
    return $base . drupal_urlencode($q) . $facet_name . drupal_urlencode($facet_value);
  }
}
/**
 * Check the given facet is the same as you search.
 */

function check_same_filter($fq, $value) {
  return ($fq == $value) ? TRUE : FALSE;
}
