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
  $escapechars = array('\\', '^', '~', ':', '(', ')', '{', '}', '[', ']' , '&&', '||', '!', '*', '?');
  foreach ($escapechars as $escchar) {
    $query = str_replace($escchar, ' ', $query);
  }
  $query = trim($query);
  $search = new OssSearch($server_details->serverurl, $server_details->autocomplete_index, 10, 0);
  $search->credential($server_details->username, $server_details->key);
  $result = $search->query($query)->template($server_details->autocomplete_query)->execute(5);
  if (isset($result) && $result instanceof SimpleXMLElement) {
    $results = new OSSResults($result);
    if ($results->getResultFound() == 0) {
      return;
    }
  if ($results->getResultFound() > 10) {
    $end = 10;
  }
  else{
    $end = $results->getResultFound();
  }
  for ($i = 0; $i < $end; $i++) {
    $suggest = stripslashes(strip_tags($results->getField($i, 'expression', FALSE)));
    if ($i > 0) {
      echo "\n";
    }
    echo trim($suggest);
    }
  }
}