<?php
/**
 * @file
 * Setting upda OpeenSearchServer admin setting
 */

$opensearchserver_cache = array();

function opensearchserver_get_settings() {
  global $opensearchserver_cache;
  if (isset($opensearchserver_cache['settings'])) {
    return $opensearchserver_cache['settings'];
  }
  $query = <<<SQL
    SELECT *  FROM {opensearchserver}
SQL;
  $stmt = db_query($query);
  $res = db_fetch_object($stmt);
  $opensearchserver_cache['settings'] = $res;
  return $res;
}

/**
 * Getting Drupal field types from database
 */

function opensearchserver_get_drupal_types() {
  global $opensearchserver_cache;
  if (isset($opensearchserver_cache['types'])) {
    return $opensearchserver_cache['types'];
  }
  $cck_fields = content_fields();
  $query_type = <<<SQL
    SELECT DISTINCT
    type,
    name,
    enabled,
    customfields
    FROM {opensearchserver_type}
SQL;
  $stmt = db_query($query_type);
  $oss_types = array();
  while ($oss_type = db_fetch_object($stmt)) {
    if ($oss_type->enabled == 1) {
      $cck_array = array();
      foreach (explode('|',$oss_type->customfields) as $custom_field) {
        $cck_field = $cck_fields[$custom_field];
        if (isset($cck_field) && $cck_field != null) {
          $cck_array[$custom_field] = $cck_field;
        }
      }
      $oss_types[$oss_type->type] = array('oss' => $oss_type, 'cck' => $cck_array);
    }
  }
  $opensearchserver_cache['types'] = $oss_types;
  return $oss_types;
}

/**
 * Getting Drupal field types from database
 */

function opensearchserver_get_drupal_categories() {
  global $opensearchserver_cache;
  if (isset($opensearchserver_cache['category'])) {
    return $opensearchserver_cache['category'];
  }
  $query_categories = <<<SQL
    SELECT
    DISTINCT
    `id`,
    `categories`,
    `enabled`
    FROM
    {opensearchserver_categories}
SQL;
  $stmt = db_query($query_categories);
  $oss_taxonomies = array();
  while ($oss_taxonomy = db_fetch_object($stmt)) {
    if ($oss_taxonomy->enabled == 1) {
      $oss_taxonomies[$oss_taxonomy->categories] = $oss_taxonomy;
    }
  }
  $opensearchserver_cache['category'] = $oss_taxonomies;
  return $oss_taxonomies;
}

/**
 * Implementing OpenSearchServer admin form for settings.
 */
function opensearchserver_admin() {

  //OpenSearchServer fieldset for forms

  $form['details'] = array(
    '#type'  => 'fieldset',
    '#title'  => t('Settings'),
    '#collapsible'  => FALSE,
    '#collapsed'  => FALSE,
  );
  $form['fields'] = array(
    '#type'  => 'fieldset',
    '#title'  => t('Types and Options'),
    '#description'  => t('Checking boxes will include the content types in the index'),
    '#collapsible'  => TRUE,
    '#collapsed'  => FALSE,
  );
  $form['categories'] = array(
    '#type'  => 'fieldset',
    '#title'  => t('Taxonomy to be indexed'),
    '#description'  =>  t('Check boxes for the taxonomy you wish to index'),
    '#collapsible'  => TRUE,
    '#collapsed'  => FALSE,
  );
  $form['autocomplete'] = array(
    '#type'  => 'fieldset',
    '#title'  => t('AutoComplete Settings'),
    '#collapsible'  => TRUE,
    '#collapsed'  => FALSE,
  );
  $form['snippet'] = array(
    '#type' => 'fieldset',
    '#title' => t('Maximum Snippets Size'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );
  $form['log'] = array(
    '#type' => 'fieldset',
    '#title' => t('Search Logs'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );
  $form['block'] = array(
    '#type' => 'fieldset',
    '#title' => t('Block tool-tip text'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );
  $form['date'] = array(
    '#type' => 'fieldset',
    '#title' => t('Date'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );

  //Get the OpenSearchServer url, indexname, username, keyGet the OpenSearchServer url, indexname, username, key.

  $settings = opensearchserver_get_settings();
  $form['details']['serverurl'] = array(
    '#type'  => 'textfield',
    '#title'  => t('OpenSearchServer Location'),
    '#description'  => t('No trailing slash at end eg:http://example.com:8080 '),
    '#default_value'  => check_url($settings->serverurl),
  );
  $form['details']['indexname'] = array(
    '#type'  => 'textfield',
    '#title'  => t('OpenSearchServer Index Name '),
    '#description'  => t('Name of the OpenSearchServer Index '),
    '#default_value'  => check_plain($settings->indexname),
  );
  $form['details']['username'] = array(
    '#type'  => 'textfield',
    '#title'  => t('OpenSearchServer User Name'),
    '#description'  => t('OpenSearchServer user name is used for authentication '),
    '#default_value'  => check_plain($settings->username),
  );
  $form['details']['key'] = array(
    '#type'  => 'textfield',
    '#title'  => t('OpenSearchServer Key'),
    '#description'  => t('Used for authentication, this key is provided when a new user is created in the OpenSearchServer instance.'),
    '#default_value'  => check_plain($settings->key),
  );
  $form['autocomplete']['autocomplete_index'] = array(
    '#type'  => 'textfield',
    '#title'  => t('Autosuggest Index Name'),
    '#description'  => t('Name of the AutoComplete OpenSearchServer Index '),
    '#default_value'  => check_plain($settings->autocomplete_index),
  );
  $form['autocomplete']['autocomplete_query'] = array(
    '#type'  => 'textfield',
    '#title'  => t('Autosuggest Query Template Name'),
    '#description'  => t('Name of the AutoComplete QueryTemplate Name '),
    '#default_value'  => check_plain($settings->autocomplete_query),
  );

  /* Load type from OSS db */
  $oss_types = opensearchserver_get_drupal_types();
  foreach (node_get_types() as $drupal_type) {
    $form['fields']['type_'.$drupal_type->type] = array(
      '#type' => 'checkbox',
      '#title' => check_plain($drupal_type->name),
      '#default_value' => check_plain(array_key_exists($drupal_type->type, $oss_types) ? 1 : 0),
    );
  }

  foreach ($oss_types as $oss_type) {
    $form['fields']['typecustoms_'.$oss_type['oss']->type] = array(
      '#type'  => 'textfield',
      '#title'  => 'Custom fields for '.$oss_type['oss']->name,
      '#description'  => t('List of custom fields to index (separated by pipe)'),
      '#default_value' => $oss_type['oss']->customfields,
    );
  }

  $form['fields']['delay'] = array(
    '#type' => 'select',
    '#title' => 'Select the Indexing interval (in minutes)',
    '#options' => array(15 => t('15'), 30 => t('30'),  60 => t('60')),
    '#default_value' => check_plain($settings->delay),
  );
  $form['fields']['signature'] = array(
    '#type' => 'checkbox',
    '#description' => t('Checking this box will include author name in the search result.'),
    '#title' => t('Signature'),
    '#default_value' => check_plain($settings->signature),
  );
  $form['fields']['filters'] = array(
    '#type' => 'checkbox',
    '#description' => t('Checking this box will allow to filter the search results based on Types, Categories and Dates.'),
    '#title' => t('Enable Faceting'),
    '#default_value' => check_plain($settings->filter_enabled),
  );
  $form['fields']['noresult_filter_enable'] = array(
    '#type' => 'checkbox',
    '#description' => t('Checking this box will display filters when the search result was zero.'),
    '#title' => t('Enable filters at zero result'),
    '#default_value' => check_plain($settings->noresult_filter_enable),
  );
  $form['fields']['enable_language_filter'] = array(
    '#type' => 'checkbox',
    '#description' => t('Checking this box will enable language filter.'),
    '#title' => t('Enable filters for locale'),
    '#default_value' => check_plain($settings->enable_language_filter),
  );
  $categories_data = db_query("
    SELECT
    name
    FROM
    {term_data}"
  );

  /* Load taxonomies from OSS db */
  $oss_taxonomies = opensearchserver_get_drupal_categories();
  foreach (taxonomy_get_vocabularies() as $vocabulary) {
    $form['categories']['taxo_'.$voc_name = str_replace(' ', '_', $vocabulary->name)] = array(
      '#type' => 'checkbox',
      '#title' => check_plain($vocabulary->name),
      '#default_value' => check_plain(array_key_exists($vocabulary->name, $oss_taxonomies) ? 1 : 0),
    );
  }

  //creating form fields for snippet
  $form['snippet']['title_snippet'] = array(
    '#type' => 'textfield',
    '#title' => t('Title Snippet Length '),
    '#description' => t('Title Snippet maximum number of characters'),
    '#default_value' => check_plain($settings->title_snippet),
  );
  $form['snippet']['content_snippet'] = array(
    '#type' => 'textfield',
    '#title' => t('Content Snippet Length '),
    '#description' => t('Content Snippet maximum number of characters'),
    '#default_value' => check_plain($settings->content_snippet),
  );
  $form['snippet']['url_snippet'] = array(
    '#type' => 'textfield',
    '#title' => t('URL Snippet Length '),
    '#description' => t('URL Snippet maximum number of characters'),
    '#default_value' => check_plain($settings->url_snippet),
  );
  //creating date field to index

  $form['date']['timestamp'] = array(
    '#type' => 'select',
    '#title' => 'Select the Date to include in index',
    '#options' => array(createddate => t('Creation Date'), changeddate => t('Modification Date')),
    '#default_value' => check_plain($settings->date),
  );
  $form['date']['date_filters'] = array(
    '#type' => 'checkbox',
    '#description' => t('Checking this box will enable Date filtering'),
    '#title' => t('Enable Date Filter'),
    '#default_value' => check_plain($settings->date_filter),
  );
  //creating log field
  $form['log']['log_enable'] = array(
    '#type' => 'checkbox',
    '#description' => t('Checking this box will log all Search queries with visitors id for further analysis.<br/> Search logs will be saved in a daily file called   "report_[indexname].[currentDate]" <br/>and will be stored in log folder of the index directory.'),
    '#title' => t('Enable Log for searching.'),
    '#default_value' => check_plain($settings->log_enable),
  );
  //creatings fields to block some text from search
  $form['block']['block_tip'] = array(
    '#type' => 'textfield',
    '#description' => t('Enter the tool tip text to be blocked.'),
    '#title' => t('Enter the tool tip text.'),
    '#default_value' => check_plain($settings->block_text),
  );
  //Added subbmit button.
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Create index'),
    '#name' => 'create',
  );
  $form['Save'] = array(
    '#type' => 'submit',
    '#value' => t('Save settings'),
    '#name' => 'save',
  );
  $form['StartIndexing'] = array(
    '#type' => 'submit',
    '#value' => t('Re-index site'),
    '#name' => 'index',
  );
  return $form;
}

/**
 * Implements drupal search form.
 */

function opensearchserver_page_form() {
  $settings = opensearchserver_get_settings();
  $arg = arg(NULL, $_GET['q']);
  if ($arg[2] != NULL) {
    $value = $arg[2];
  }
  else {
    $value='';
  }
  $form['basic']['inline'] = array('#prefix' => '<div class="container-inline">', '#suffix' => '</div><div id="autocomplete"></div>');
  if ($settings->autocomplete_index && $settings->autocomplete_query) {
    $clean_url = variable_get('clean_url', '0');
    $form['basic']['inline']['q'] = array(
      '#type' => 'textfield',
      '#title' => '',
      '#default_value' => $value,
      '#maxlength' => 255,
      '#attributes' => array('onkeyup' => 'return OpenSearchServer.autosuggest(event ,'. $clean_url . ')', 'autocomplete' => 'off'),
    );
  }
  else {
    $form['basic']['inline']['q'] = array(
      '#type' => 'textfield',
      '#title' => '',
      '#default_value' => $value,
      '#maxlength' => 255,
    );
  }
  $form['basic']['inline']['submit'] = array('#type' => 'submit', '#value' => t('Search'));
  return $form;
}

/**
 * Implementing form validation for OpenSearchServer admin settings.
 */

function opensearchserver_admin_validate($form_id, &$form_state) {
  $index_name = $form_state['values']['indexname'];
  $server_url = $form_state['values']['serverurl'];
  $username = $form_state['values']['username'];
  $key = $form_state['values']['key'];
  $autocomplete_index = $form_state['values']['autocomplete_index'];
  $autocomplete_query = $form_state['values']['autocomplete_query'];
  if ($form_state['clicked_button']['#name']=='save') {
    if (empty($index_name)) {
      form_set_error('Index name', t('Please enter a valid index name / Bad credential to login to server'));
    }
    if (empty($server_url)) {
      form_set_error('Serverurl', t('Please enter a valid server url / Bad credential to login to server'));
    }
    if (!empty($autocomplete_index) && !empty($autocomplete_query)) {
      $oss_api = get_oss_api($server_url, $username, $key, NULL);
      if (!$oss_api->isIndexAvailable($autocomplete_index, $autocomplete_query)) {
        form_set_error('Autocomplete', t('Please enter a valid autocomplete index and query template/ Bad credential to login to server'));
      }
    }
  }
}

/**
 * Getting OpenSearchServer API instance
 */

function opensearchserver_get_oss_api($serverurl, $username, $key, $indexname) {
  if ($indexname) {
    $oss_api = new OssApi($serverurl, $indexname);
  }
  else {
    $oss_api = new OssApi($serverurl);
  }
  $oss_api->credential($username, $key);
  return $oss_api;
}

/**
 * Creating an index in OpenSearchServer instance
 */

function opensearchserver_create_index($serverurl, $indexname, $username, $key) {
  $oss_api = opensearchserver_get_oss_api($serverurl, $username, $key, '');
  $oss_api->createIndex($indexname);
  return TRUE;
}

/**
 * Delete all documents from the index.
 */

function opensearchserver_delete_all() {
  $oss_settings = opensearchserver_get_settings();
  $delete_api = new OssDelete($oss_settings->serverurl, $oss_settings->indexname);
  $delete_api->credential($oss_settings->username, $oss_settings->key);
  $delete_api->delete('*:*');
}
/**
 * Implementing form submittion for OpenSearchServer admin settings.
 */

function opensearchserver_admin_submit($form_id, &$form_state) {
  $indexname = $form_state['values']['indexname'];
  $serverurl = $form_state['values']['serverurl'];
  $username = $form_state['values']['username'];
  $key = $form_state['values']['key'];

  switch ($form_state['clicked_button']['#name']) {
    case 'create':
      opensearchserver_create_index($serverurl, $indexname, $username, $key);
    case 'save':
      opensearchserver_drupal_schema($serverurl, $indexname, $username,  $key);
      opensearchserver_insert_database_values($form_state, $serverurl, $indexname, $username, $key);
      break;
    case 'index':
      opensearchserver_delete_all();
      $batch = array(
        'title' => t('Indexing the document'),
        'operations' => array(array('opensearchserver_generate_documents_to_index', array($info))),
      );
      batch_set($batch);
      batch_process('admin/settings/opensearchserver');
      drupal_set_message(t('Documents had been indexed'));
      break;
  }
}

/**
 * Truncate all the values from database
 */

function opensearchserver_truncate_db() {
  db_query("
    TRUNCATE
    TABLE
    {opensearchserver}"
    );
    db_query("
      TRUNCATE
      TABLE
      {opensearchserver_type}"
      );
      db_query("
        TRUNCATE
        TABLE
        {opensearchserver_categories}"
        );
}
/**
 * Inserting the settings value to database
 */

function opensearchserver_insert_database_values($form_state, $serverurl, $indexname, $username,  $key) {
  opensearchserver_truncate_db();
  db_query("
    INSERT INTO `{opensearchserver}`
    (
    `serverurl`,
    `indexname`,
    `username`,
    `key`,
    `last_indexed`,
    `delay`,
    `filter_enabled`,
    `signature`,
    `title_snippet`,
    `content_snippet`,
    `url_snippet`,
    `date`,
    `date_filter`,
    `log_enable`,
    `noresult_filter_enable`,
    `enable_language_filter`,
    `block_text`,
    `autocomplete_index`,
    `autocomplete_query`
    )
    VALUES
    ('%s',  '%s',  '%s',  '%s',  '%s',  '%d',  '%d', '%d', '%d', '%d', '%d', '%s', '%d', '%d', '%d','%d', '%s', '%s', '%s')",
    $form_state['values']['serverurl'],
    $form_state['values']['indexname'],
    $form_state['values']['username'],
    $form_state['values']['key'], '',
    $form_state['values']['delay'],
    $form_state['values']['filters'],
    $form_state['values']['signature'],
    $form_state['values']['title_snippet'],
    $form_state['values']['content_snippet'],
    $form_state['values']['url_snippet'],
    $form_state['values']['timestamp'],
    $form_state['values']['date_filters'],
    $form_state['values']['log_enable'],
    $form_state['values']['noresult_filter_enable'],
    $form_state['values']['enable_language_filter'],
    $form_state['values']['block_tip'],
    $form_state['values']['autocomplete_index'],
    $form_state['values']['autocomplete_query']
  );

  foreach (node_get_types() as $type) {
    db_query("
      INSERT INTO
      `{opensearchserver_type}`
      (
      `type`, `name`, `enabled`, `customfields`
      )
      VALUES
      (
      '%s', '%s', '%d', '%s'
      )",
      $type->type,
      $type->name,
      $form_state['values']['type_'.$type->type],
      $form_state['values']['typecustoms_'.$type->type]
    );
  }

  foreach (taxonomy_get_vocabularies() as $vocabulary) {
    db_query("
      INSERT INTO `{opensearchserver_categories}`
      (
      `id`, `categories`,  `enabled`
      )
      VALUES
      (
      '%s', '%s',  '%d')",
      $vocabulary->vid, $vocabulary->name,
      $form_state['values']['taxo_'.str_replace(' ',  '_',  $vocabulary->name)]
    );
  }
  drupal_set_message(t('Settings has been Updated'));
}

function opensearchserver_get_field_name($oss_type, $cck_field) {
  return 'type_'.$oss_type['oss']->type.'_'.$cck_field['field_name'];

}
/**
 * Implements the field creation on schema of OpenSearchServer.
 */

function opensearchserver_drupal_schema($url,  $indexname,  $username,  $key) {
  $settings = opensearchserver_get_settings();
  $oss_api = opensearchserver_get_oss_api($url, $username, $key, $indexname);
  $oss_api->setField('id', '', 'NO', 'YES', 'NO', 'NO', 'YES');
  $oss_api->setField('lang', '', 'NO', 'YES', 'NO', 'NO', 'NO');
  $oss_api->setField('type', '', 'NO', 'YES', 'NO', 'NO', 'NO');
  foreach (opensearchserver_get_drupal_categories() as $oss_taxonomy) {
    $oss_api->setField('taxo_'.$oss_taxonomy->id, '', 'NO', 'YES', 'NO', 'NO', 'NO');
  }

  // Set fields from CCK
  foreach (opensearchserver_get_drupal_types() as $oss_type) {
    if ($oss_type['oss']->enabled) {
      foreach ($oss_type['cck'] as $cck_field) {
        switch ($cck_field['type']) {
          case 'text':
            $analyzer = 'TextAnalyzer';
            break;
          case 'number_decimal':
            $analyzer = 'DecimalAnalyzer';
            break;
          default:
            $analyzer = '';
            break;
        }
        $oss_api->setField(opensearchserver_get_field_name($oss_type, $cck_field), $analyzer, 'YES', 'YES', 'NO', 'NO', 'NO');
      }
    }
  }

  $oss_api->setField('content_timestamp', '', 'NO', 'YES', 'NO', 'NO', 'NO');
  $oss_api->setField('url', '', 'NO', 'YES', 'NO', 'NO', 'NO');
  $oss_api->setField('title', 'TextAnalyzer', 'NO', 'YES', 'positions_offsets', 'NO', 'NO');
  $oss_api->setField('autocomplete', 'AutoCompletionAnalyzer', 'NO', 'YES', 'NO', 'NO', 'NO');
  $oss_api->setField('content', 'TextAnalyzer', 'compress', 'YES', 'positions_offsets', 'YES', 'NO');
  $oss_api->setField('timestamp', '', 'NO', 'YES', 'NO', 'NO', 'NO');
  $oss_api->setField('comments_subject', 'TextAnalyzer', 'NO', 'YES', 'positions_offsets', 'NO', 'NO');
  $oss_api->setField('comments_comment', 'TextAnalyzer', 'compress', 'YES', 'positions_offsets', 'NO', 'NO');
  $oss_api->setField('user_name', 'TextAnalyzer', 'NO', 'YES', 'positions_offsets', 'NO', 'NO');
  $oss_api->setField('user_email', 'TextAnalyzer', 'NO', 'YES', 'positions_offsets', 'NO', 'NO');
  $oss_api->setField('user_url', '', 'NO', 'YES', 'NO', 'NO', 'NO');
  $oss_api->setField('full', 'TextAnalyzer', 'NO', 'YES', 'positions_offsets', 'NO', 'NO');
  $oss_api->setField('full_exact', 'StandardAnalyzer', 'NO', 'YES', 'positions_offsets', 'NO', 'NO');
  $search_template=new OssSearchTemplate($url, $indexname);
  $search_template->credential($username, $key);
  $search_template->createSearchTemplate('search', '
    title:($$)^10 OR title:("$$")^10 OR content:($$)^2 OR content:("$$")^2 OR
    full:($$) OR full:("$$") OR full_exact:($$) OR full_exact:("$$") OR
    comments_subject:($$)^0.2 OR comments_subject:("$$")^0.2 OR
    comments_comment:($$)^0.1 OR comments_comment:("$$")^0.1',
    "AND", "10", "2", "ENGLISH");
  $search_template->setSnippetField('search', "title", $settings->title_snippet, "b", "1", "NoFragmenter");
  $search_template->setSnippetField('search', "content", $settings->content_snippet, "b", "1", "SentenceFragmenter");
  $search_template->setReturnField('search', "url");
  $search_template->setReturnField('search', "user_url");
  $search_template->setReturnField('search', "type");
  foreach (opensearchserver_get_drupal_categories() as $oss_taxonomy) {
    $search_template->setReturnField('search', 'taxo_'.$oss_taxonomy->id);
  }
  $search_template->setReturnField('search', "lang");
  $search_template->setReturnField('search', "content_timestamp");
  $search_template->setSnippetField('search', "comments_comment", "200", "b", "1", "SentenceFragmenter");
  $search_template->setSnippetField('search', "comments_subject", "150", "b", "1", "NoFragmenter");
  $search_template->setSnippetField('search', "user_name", "70", "b", "1", "NoFragmenter");
  $search_template->setSnippetField('search', "user_email", "70", "b", "1", "NoFragmenter");
}
/**
 * Get total documents from db
 */
function opensearchserver_get_total_result($type) {
  $count_result_query=<<<SQL
    SELECT
    COUNT(*) FROM
    {node} n, {node_revisions} rev
    WHERE
    n.nid = rev.nid AND n.type LIKE '%s'
SQL;
  return db_query($count_result_query, $type);
}

/**
 * Generate SQL query to get all the documents
 */
function opensearchserver_get_allpagest_db($createddate) {
  if ($createddate) {
    $sql = <<<SQL
      SELECT node.nid
      FROM {node}
      WHERE node.type = '%s' AND node.status = %d
      LIMIT %d, %d
SQL;
  } else {
    $sql = <<<SQL
    SELECT node.nid,
    FROM {node}
    WHERE node.type = '%s' AND node.status = %d
    LIMIT %d, %d
SQL;
  }
  return $sql;
}


/*
 * Get url alise if available
*/
function opensearchserver_get_url_alias($nid) {
  $urlalise = db_query("
    SELECT dst
    FROM
    {url_alias}
    WHERE
    url_alias.src='%s'", 'node/' . $nid
  );
  if (module_exists('locale')) {
    $url_prefix = db_query("
      SELECT language,
      enabled,prefix
      FROM
      {languages}
      WHERE
      languages.language='%s' AND languages.enabled='%d'", $resultdoc->language, '1'
    );
    $url_prefix_object = db_fetch_object($url_prefix);
  }
  $urlalise_object = db_fetch_object($urlalise);
  if ($urlalise_object) {
    $url_alise = $url_prefix_object->prefix ? $url_prefix_object->prefix . '/' . $urlalise_object->dst : $urlalise_object->dst;
    $url = url($url_alise, array('absolute' => TRUE));
  }
  else {
    $url_with_lang = 'node/'. $nid;
    $url = url($url_with_lang, array('absolute' => TRUE));
  }
  return $url;
}

/**
 * Creating documents for index
 */

function opensearchserver_create_index_document($document, $nid , $language, $createddate, $oss_type) {
  global $base_url;
  $node = node_load($nid);
  $document->newField('id', $node->type . '_' . $node->nid);
  if ($createddate) {
    $document->newField('content_timestamp', date('YmdHis', $node->created));
  } else {
    $document->newField('content_timestamp', date('YmdHis', $node->changed));
  }

  // Load taxonomy
  if (!empty($node->taxonomy)) {
    foreach (opensearchserver_get_drupal_categories() as $oss_taxonomy) {
      $field = $document->newField('taxo_'.$oss_taxonomy->id);
      foreach ($node->taxonomy as $term) {
        if ($term->vid == $oss_taxonomy->id) {
          $field->newValue(html_entity_decode($term->name, ENT_QUOTES, 'UTF-8'), TRUE);
        }
      }
    }
  }

  $document->newField('lang')->newValue(html_entity_decode($node->language, ENT_QUOTES, 'UTF-8'), TRUE);
  $document->newField('type')->newValue(html_entity_decode($node->type, ENT_QUOTES, 'UTF-8'), TRUE);
  $title = html_entity_decode($node->title, ENT_QUOTES, 'UTF-8');
  $document->newField('title')->newValue($title, TRUE);
  $document->newField('full')->newValue($title, TRUE);
  $document->newField('full_exact')->newValue($title, TRUE);
  $content = html_entity_decode($node->body, ENT_QUOTES, 'UTF-8');
  $document->newField('content')->newValue($content, TRUE);
  $document->newField('full')->newValue($content, TRUE);
  $document->newField('full_exact')->newValue($content, TRUE);
  $document->newField('autocomplete')->newValue(html_entity_decode($node->body, ENT_QUOTES, 'UTF-8'), TRUE);
  $document->newField('autocomplete')->newValue(html_entity_decode($node->title, ENT_QUOTES, 'UTF-8'), TRUE);
  $document->newField('url', opensearchserver_get_url_alias($node->nid));
  $document->newField('timestamp', $node->revision_timestamp);
  $user_url = drupal_get_path_alias('user/' . $node->uid);
  $user = user_load($node->uid);
  $document->newField('user_name', $user->name);
  $document->newField('user_email', $user->mail);
  $document->newField('user_url', url($user_url , array('absolute' => TRUE)));

  // Load custom fields
  foreach ($oss_type['cck'] as $cck_field) {
    $oss_field_name = opensearchserver_get_field_name($oss_type, $cck_field);
    foreach ($node->$cck_field['field_name'] as $field_value) {
      $value = html_entity_decode($field_value['value'], ENT_QUOTES, 'UTF-8');
      $document->newField($oss_field_name)->newValue($value, TRUE);
      $document->newField('full')->newValue($value, TRUE);
      $document->newField('full_exact')->newValue($value, TRUE);
    }
  }

  return $document;
}

/**
 * Generate documents for indexing.
 */
function opensearchserver_generate_documents_to_index() {
  global $language;
  $limit = 100;
  if (!isset($langcode)) {
    $langcode = $GLOBALS['language_content']->language;
  }
  foreach (opensearchserver_get_drupal_types() as $oss_type) {
    $current_type_count = db_result(opensearchserver_get_total_result($oss_type['oss']->type));
    if($current_type_count >100) {
      for ($i = 0; $i <= $current_type_count; $i++) {
        if ($i != 0) {
          $i = $j + 1;
        }
        $j = $i + $limit;
        $index = opensearchserver_iterate_the_documents($i , $limit, $language, $oss_type);
        opensearchserver_start_indexing($index);
        watchdog('OpenSearchServer',$i. ' documents indexed');
      }
    }
    else{
      $index = opensearchserver_iterate_the_documents(0 , $current_type_count, $language, $oss_type);
      opensearchserver_start_indexing($index);
      watchdog('OpenSearchServer',$current_type_count. ' documents indexed');
    }
  }
}

/*
 * Iterate over the documents
*/
function opensearchserver_iterate_the_documents($from, $limit, $language, $oss_type) {
  $createddate = opensearchserver_get_settings()->date == "createddate";
  $sql = opensearchserver_get_allpagest_db($createddate);
  $result = db_query($sql, $oss_type['oss']->type, 1, $from, $limit);

  $index = new OssIndexDocument();
  while ($resultdoc = db_fetch_object($result)) {
    $document = $index->newDocument($language->language);
    opensearchserver_create_index_document($document, $resultdoc->nid, $language->language, $createddate, $oss_type);
  }
  return $index;
}

/**
 * Generate comment documents for indexing.
 */

function opensearchserver_get_comment_documents($resultdoc) {
  if ($resultdoc->comment > 0) {
    $comments = db_query("
      SELECT
      cid, nid, subject, timestamp, comment
      FROM {comments}
      WHERE
      comments.nid = %d", $resultdoc->nid
    );
    while ($commentdoc = db_fetch_object($comments)) {
      $document = $index->newDocument($language->language);
      $comment_url = drupal_get_path_alias('user/' . $resultdoc->nid . '#comment-' . $commentdoc->cid);
      $document->newField('id', 'comment_' . $commentdoc->cid);
      $document->newField('type', 'comments');
      $document->newField('lang', $resultdoc->language);
      $document->newField('comments_subject')->newValue(html_entity_decode($resultdoc->subject, ENT_QUOTES, 'UTF-8'), TRUE);
      $document->newField('comments_comment')->newValue(html_entity_decode($resultdoc->comment, ENT_QUOTES, 'UTF-8'), TRUE);
      $document->newField('url', url($comment_url , array('absolute' => TRUE)));
      $document->newField('timestamp', $commentdoc->timestamp);
      $user=user_load($resultdoc->uid);
      $document->newField('user_name', $user->name);
      $document->newField('user_email', $user->mail);
      $user_url = drupal_get_path_alias('user/' . $resultdoc->uid);
      $document->newField('user_url', url($user_url , array('absolute' => TRUE)));
      opensearchserver_start_indexing($index);
    }
  }
}
/**
 * Start indexing process
 */

function opensearchserver_start_indexing($index) {
  $oss_settings = opensearchserver_get_settings();
  $server = new OssApi($oss_settings->serverurl, $oss_settings->indexname);
  $server->credential($oss_settings->username, $oss_settings->key);
  if ($server->update($index, $oss_settings->indexname) === FALSE) {
    $errors[] = 'Failed to update';
  }
  $server->optimize();
}