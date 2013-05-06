<?php
/**
 * @file
 * Setting autocomplete for search
 */

function opensearchserver_autocomplete() {
  $query=isset($_REQUEST['query']) ? $_REQUEST['query'] : NULL;
  $oss_settings = opensearchserver_get_settings();
  $escapechars = array('\\', '^', '~', ':', '(', ')', '{', '}', '[', ']' , '&&', '||', '!', '*', '?');
  foreach ($escapechars as $escchar) {
    $query = str_replace($escchar, ' ', $query);
  }
  $query = trim($query);
  $search = new OssSearch($oss_settings->serverurl, $oss_settings->autocomplete_index, 10, 0);
  $search->credential($oss_settings->username, $oss_settings->key);
  $result = $search->query($query)->template($oss_settings->autocomplete_query)->execute(5);
  if (isset($result) && $result instanceof SimpleXMLElement) {
    $results = new OSSResults($result);
    if ($results->getResultFound() == 0) {
      return;
    }
    $num_found = $results->getResultFound();
    $end = ($num_found > 10) ? 10 : $num_found;
    for ($i = 0; $i < $end; $i++) {
      $suggest = stripslashes(strip_tags($results->getField($i, 'expression', FALSE)));
      if ($i > 0) {
        echo "\n";
      }
      echo trim($suggest);
    }
  }
}