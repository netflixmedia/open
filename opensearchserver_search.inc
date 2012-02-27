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

function opensearchserver_preprocess_search_theme_form(&$vars, $hook)  {
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
 * Implements form validation.
 */

function opensearchserver_validate($form, &$form_state) {
  if (empty($form_state['values']['keys'])) {
    form_set_error('keywords', t('Please enter a search term.'));
  }
  if (empty($form_state['values']['search_theme_form'])) {
    form_set_error('keywords', t('Please enter a search term.'));
  }
  if (empty($form_state['values']['search_block_form'])) {
    form_set_error('keywords', t('Please enter a search term.'));
  }
}

/**
 * Implements form submission.
 */

function opensearchserver_submit($form, &$form_state) {
  if ($form_state['values']['search_theme_form']) {
    $form_state['redirect'] = 'opensearchserver/search/'. check_plain($form_state['values']['search_theme_form']);
  }
  elseif ($form_state['values']['search_block_form']) {
    $form_state['redirect'] = 'opensearchserver/search/'. check_plain($form_state['values']['search_block_form']);
  }
  else {
    $form_state['redirect'] = 'opensearchserver/search/'. check_plain($form_state['values']['keys']);
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
    signature
    FROM
    {opensearchserver}"
  );
  $search_settings=db_fetch_object($block_tool);
  $tq = isset($_REQUEST['tq']) ? $_REQUEST['tq'] : NULL;
  $fq = isset($_REQUEST['fq']) ? $_REQUEST['fq'] : NULL;
  $ts = isset($_REQUEST['ts']) ? $_REQUEST['ts'] : NULL;
  $opensearchserver_data['q']=$arg[2];
  $result=opensearchserver_get_search_result($arg[2], TRUE);
  $result_facet=opensearchserver_get_search_result($arg[2], FALSE);
  $opensearchserver_data['paging']=opensearchserver_paging($result);
  $timestamp_array = array("past24" => "Past 24 hours", "pastweek" => "Past week", "pastmonth" => "Past month", "pastyear" => "Past year");
  $opensearchserver_data['tq'] = $tq;
  $opensearchserver_data['fq'] = $fq;
  $opensearchserver_data['ts'] = $ts;
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
  return theme('opensearchserver_search', $opensearchserver_data);
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

/**
 * Get the search result from the index.
 */

function opensearchserver_get_search_result($query, $facet) {
  if ($query) {
    global $user;
    $get_details = db_query("
      SELECT *
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
    if ($facet) {
      $filter=$_REQUEST['fq'];
      if ($filter) {
        if ($filter != 'All') {
          $search->filter('type:"' . $filter . '"');
        }
      }
      $filter_categories=$_REQUEST['tq'];
      if ($filter_categories) {
        if ($filter_categories != 'All') {
          $search->filter('taxonomy:"' . $filter_categories . '"');
        }
      }
      $timestamp_filter=$_REQUEST['ts'];
      if ($timestamp_filter) {
        if ($timestamp_filter != 'All') {
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
    }
    $result = $search->query($query)->template('search')->execute(5);
  }
  return $result;
}

/**
 * Implements drupal search form submit.
 */

function opensearchserver_page_form_submit($form_id, &$form_state) {
  $form_state['redirect'] = 'opensearchserver/search/'. $form_state['values']['q'];
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
  return '<a id="oss_facet" href="/?q=opensearchserver/search/'. drupal_urlencode($query) .'">Everything</a>';
}