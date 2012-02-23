<?php
/**
 * @file
 * Setting autocomplete for search
 */

function opensearchserver_autocomplete() {
  $query=isset($_REQUEST['query']) ? $_REQUEST['query'] : NULL;
  $get_details = db_query("
        SELECT *
        FROM {opensearchserver}"
  );
  $server_details = db_fetch_object($get_details);
  if ($server_details->autocomplete_index && $server_details->autocomplete_query) {
  //Add the autocomplete javascript
  drupal_add_js(drupal_get_path('module', 'OpenSearchServer') . '/js/opensearchserver.js');
  $vars['scripts'] = drupal_get_js();
    $escapechars = array('\\', '^', '~', ':', '(', ')', '{', '}', '[', ']' , '&&', '||', '!', '*', '?');
    foreach ($escapechars as $escchar) {
      $query = str_replace($escchar, ' ', $query);
    }
    $query = trim($query);
    $search = new OssSearch($server_details->serverurl, $server_details->autocomplete_index, 10, 0);
    $search->credential($server_details->username, $server_details->key);
    $result = $search->query($query)->template($server_details->autocomplete_query)->execute(5);
    $results = new OSSResults($result);
    if ($results->getResultFound() == 0) {
      return;
    }
  for ($i = 0; $i < 10; $i++) {
    $suggest = stripslashes(strip_tags($results->getField($i, 'expression', TRUE)));
    echo $suggest;
    echo "\r\n";
  }
  }
  }
