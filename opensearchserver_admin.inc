<?php
/**
 * @file
 * Setting upda OpeenSearchServer admin setting
 */
function get_settings_query() {
  $query = <<<SQL
    SELECT `serverurl`,
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
    FROM {opensearchserver}
SQL;
  return db_query($query);
}

/**
 * Getting Drupal field types from database
 */

function get_drupal_types() {
  $query_type = <<<SQL
    SELECT DISTINCT
    type,
    name,
    enabled
    FROM {opensearchserver_type}
SQL;
  return db_query($query_type);
}
/**
 * Getting Drupal field types from database
 */

function get_drupal_categories() {
  $query_categories = <<<SQL
    SELECT
    DISTINCT
    `categories`,
    `enabled`
    FROM
    {opensearchserver_categories}
SQL;
  return db_query($query_categories);
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
    '#title'  => t(' Fields to be indexed'),
    '#description'  => t('Checking boxes will include the content types in the index'),
    '#collapsible'  => TRUE,
    '#collapsed'  => FALSE,
  );
  $form['categories'] = array(
    '#type'  => 'fieldset',
    '#title'  => t('Categories to be indexed'),
    '#description'  =>  t('Check boxes for the categories you wish to index'),
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

  $settings = db_fetch_object(get_settings_query());
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
    '#description'  => t('Used for authentication, this key is provided when a new user is  created  in the OpenSearchServer instance.'),
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
  //Get the field type to be indexed
  $count = db_result(db_query("
    SELECT
    COUNT(*)
    FROM
    {opensearchserver_type}"
  ));

  $types_from_ossdb = get_drupal_types();
  $result = db_query("
    SELECT
    name, type
    FROM
    {node_type}"
  );

  if ($count > 0) {
    while ($drupal_types = db_fetch_object($types_from_ossdb)) {
      $form['fields'][$drupal_types->type] = array(
        '#type' => 'checkbox',
        '#title' => check_plain($drupal_types->name),
        '#default_value' => check_plain($drupal_types->enabled),
      );
    }
  }
  else {
    while ($results = db_fetch_object($result)) {
      $form['fields'][$results->type] = array(
        '#type' => 'checkbox',
        '#title' => check_plain($results->name),
      );
    }
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
    {vocabulary}"
  );

  $category_count = db_result(db_query("
    SELECT
    COUNT(*)
    FROM
    {opensearchserver_categories}"
  ));

  $category_type = db_query("
    SELECT
    DISTINCT
    id, categories, enabled
    FROM
    {opensearchserver_categories}"
  );

  if ($category_count > 0) {
    while ($cate = db_fetch_object($category_type)) {
      $form['categories'][str_replace(' ', '_', $cate->categories)] = array(
        '#type' => 'checkbox',
        '#title' => check_plain($cate->categories),
        '#default_value' => check_plain($cate->enabled),
      );
    }
  }
  else {
    while ($category = db_fetch_object($categories_data)) {
      $form['categories'][str_replace(' ', '_', $category->name)] = array(
        '#type' => 'checkbox',
        '#title' => check_plain($category->name),
      );
    }
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
    '#value' => t('Create-index / Save'),
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
  $settings = db_fetch_object(get_settings_query());
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

function get_oss_api($serverurl, $username, $key, $indexname) {
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

function create_index($serverurl, $indexname, $username, $key) {
  $oss_api = get_oss_api($serverurl, $username, $key, '');
  $oss_api->createIndex($indexname);
  return TRUE;
}

/**
 * Delete all documents from the index.
 */

function delete_all() {
  $delete_query = <<<SQL
    SELECT serverurl,
    `indexname`,
    `username`,
    `key`
    FROM {opensearchserver}
SQL;
  $get_server_details = db_query($delete_query);
  $server_details = db_fetch_object($get_server_details);
  $delete_api = new OssDelete($server_details->serverurl, $server_details->indexname);
  $delete_api->credential($server_details->username, $server_details->key);
  $delete_api->delete('*:*');
}
/**
 * Implementing form submittion for OpenSearchServer admin settings.
 */

function opensearchserver_admin_submit($form_id, &$form_state) {
  switch ($form_state['clicked_button']['#name']) {
    case 'save':
      $indexname = $form_state['values']['indexname'];
      $serverurl = $form_state['values']['serverurl'];
      $username = $form_state['values']['username'];
      $key = $form_state['values']['key'];
      $oss_api = get_oss_api($serverurl, $username, $key, '');
      if (!$oss_api->isIndexAvailable($indexname)) {
        create_index($serverurl, $indexname, $username, $key);
        opensearchserver_drupl_schema($serverurl, $indexname, $username,  $key);
      }
      insert_database_values($form_state, $serverurl, $indexname, $username, $key);
      break;
    case 'index':
      delete_all();
      $batch = array(
        'title' => t('Indexing the document'),
        'operations' => array(array('generate_documents_to_index', array($info))),
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

function truncate_db() {
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

function insert_database_values($form_state, $serverurl, $indexname, $username,  $key) {
  truncate_db();
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
  $types = db_query("
    SELECT
    DISTINCT
    type, name
    FROM {node_type}"
  );

  while ($type = db_fetch_object($types)) {
  db_query("
    INSERT INTO
    `{opensearchserver_type}`
    (
    `type`, `name`, `enabled`
    )
    VALUES
    (
    '%s', '%s', '%d'
    )",
    $type->type,
    $type->name,
    $form_state['values'][$type->type]
  );
  }

  $categories = db_query("
    SELECT
    DISTINCT
    vid, name
    FROM
    {vocabulary}"
  );

  while ($category = db_fetch_object($categories)) {
  db_query("
    INSERT INTO `{opensearchserver_categories}`
    (
    `id`, `categories`,  `enabled`
    )
    VALUES
    (
    '%s', '%s',  '%d')",
    $category->vid, $category->name,
    $form_state['values'][str_replace(' ',  '_',  $category->name)]
  );
  }
  drupal_set_message(t('Settings has been Updated'));
}

/**
 * Implements the field creation on schema of OpenSearchServer.
 */

function opensearchserver_drupl_schema($url,  $indexname,  $username,  $key) {
  $snippet = get_settings_query();
  $snippet_value = db_fetch_object($snippet);
  $oss_api = get_oss_api($url, $username, $key, $indexname);
  $oss_api->setField('id', '', 'NO', 'YES', 'YES', '', 'NO', 'YES');
  $oss_api->setField('lang', '', 'NO', 'YES', 'YES', '', 'NO', 'NO');
  $oss_api->setField('type', '', 'NO', 'YES', 'YES', '', 'NO', 'NO');
  $oss_api->setField('taxonomy', '', 'NO', 'YES', 'YES', '', 'NO', 'NO');
  $oss_api->setField('content_timestamp', '', 'NO', 'YES', 'YES', '', 'NO', 'NO');
  $oss_api->setField('url', '', 'NO', 'YES', 'YES', '', 'NO', 'NO');
  $oss_api->setField('title', 'TextAnalyzer', 'compress', 'YES', 'positions_offsets', '', 'NO', 'NO');
  $oss_api->setField('autocomplete', 'AutoCompletionAnalyzer', 'NO', 'YES', 'NO', '', 'NO', 'NO');
  $oss_api->setField('content', 'TextAnalyzer', 'compress', 'YES', 'positions_offsets', '', 'YES', 'NO');
  $oss_api->setField('timestamp', '', 'NO', 'YES', 'YES', '', 'NO', 'NO');
  $oss_api->setField('comments_subject', 'TextAnalyzer', 'compress', 'YES', 'positions_offsets', '', 'NO', 'NO');
  $oss_api->setField('comments_comment', 'TextAnalyzer', 'compress', 'YES', 'positions_offsets', '', 'NO', 'NO');
  $oss_api->setField('user_name', 'TextAnalyzer', 'compress', 'YES', 'positions_offsets', '', 'NO', 'NO');
  $oss_api->setField('user_email', 'TextAnalyzer', 'compress', 'YES', 'positions_offsets', '', 'NO', 'NO');
  $oss_api->setField('user_url', '', 'NO', 'YES', 'YES', '', 'NO', 'NO');
  $search_template=new OssSearchTemplate($url, $indexname);
  $search_template->credential($username, $key);
  $search_template->createSearchTemplate("search", '
  title:($$)^10 OR title:("$$")^10 OR
  content:($$)^10 OR content:("$$")^10 OR
  comments_subject:($$)^10 OR comments_subject:("$$")^10 OR
  comments_comment:($$)^10 OR comments_comment:("$$")^10',
  "AND", "10", "2", "ENGLISH");
  $search_template->setSnippetField("search", "title", $snippet_value->title_snippet, "b", "1", "NoFragmenter");
  $search_template->setSnippetField("search", "content", $snippet_value->content_snippet, "b", "1", "SentenceFragmenter");
  $search_template->setReturnField("search", "url");
  $search_template->setReturnField("search", "user_url");
  $search_template->setReturnField("search", "type");
  $search_template->setReturnField("search", "taxonomy");
  $search_template->setReturnField("search", "lang");
  $search_template->setReturnField("search", "content_timestamp");
  $search_template->setSnippetField("search", "comments_comment", "200", "b", "1", "SentenceFragmenter");
  $search_template->setSnippetField("search", "comments_subject", "150", "b", "1", "NoFragmenter");
  $search_template->setSnippetField("search", "user_name", "70", "b", "1", "NoFragmenter");
  $search_template->setSnippetField("search", "user_email", "70", "b", "1", "NoFragmenter");
}
/**
 * Get total documents from db
 */
function get_total_result($type) {
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
function get_allpagest_db($create_date) {
  if ($create_date="createddate") {
    $sql = <<<SQL
      SELECT
      node.type,
      node.nid,
      node.comment,
      node.status,
      node.created,
      node.language,
      node_revisions.uid,
      node_revisions.title,
      node_revisions.body,
      node_revisions.timestamp
      FROM {node}, {node_revisions}
      WHERE
      node.nid = node_revisions.nid
      AND
      node.type LIKE '%s' LIMIT %d, %d
SQL;
  }
  else {
    $sql = <<<SQL
    SELECT
    node.type,
    node.nid,
    node.comment,
    node.status,
    node.changed,
    node.language,
    node_revisions.uid,
    node_revisions.title,
    node_revisions.body,
    node_revisions.timestamp
    FROM {node}, {node_revisions}
    WHERE
    node.nid = node_revisions.nid
    AND
    node.type LIKE '%s' LIMIT %d, %d
SQL;
  }
  return $sql;
}

/**
 * Creating documents for index
 */

function create_index_document($document, $resultdoc , $taxonomyarray_data,$language) {
  global $base_url;
  $document->newField('id', $resultdoc->type . '_' . $resultdoc->nid);
  if ($resultdoc->created) {
    $document->newField('content_timestamp', date('YmdHis', $resultdoc->created));
  }
  else {
    $document->newField('content_timestamp', date('YmdHis', $resultdoc->changed));
  }
  $document->newField('lang')->newValue(html_entity_decode($resultdoc->language, ENT_QUOTES, 'UTF-8'), TRUE);
  $document->newField('type')->newValue(html_entity_decode($resultdoc->type, ENT_QUOTES, 'UTF-8'), TRUE);
  $document->newField('taxonomy')->newValue(html_entity_decode($taxonomyarray_data->name, ENT_QUOTES, 'UTF-8'), TRUE);
  $document->newField('title')->newValue(html_entity_decode($resultdoc->title, ENT_QUOTES, 'UTF-8'), TRUE);
  $document->newField('content')->newValue(html_entity_decode($resultdoc->body, ENT_QUOTES, 'UTF-8'), TRUE);
  $document->newField('autocomplete')->newValue(html_entity_decode($resultdoc->body, ENT_QUOTES, 'UTF-8'), TRUE);
  $document->newField('autocomplete')->newValue(html_entity_decode($resultdoc->title, ENT_QUOTES, 'UTF-8'), TRUE);
  $urlalise = db_query("
    SELECT dst
    FROM
    {url_alias}
    WHERE
    url_alias.src='%s'", 'node/' . $resultdoc->nid
  );
  $url_prefix = db_query("
    SELECT language,
    enabled,prefix
    FROM
    {languages}
    WHERE
    languages.language='%s' AND languages.enabled='%d'", $resultdoc->language,'1'
  );
  $url_prefix_object = db_fetch_object($url_prefix);
  $urlalise_object = db_fetch_object($urlalise);
  if ($urlalise_object) {
    $url_alise = $url_prefix_object->prefix ? $url_prefix_object->prefix.'/'.$urlalise_object->dst : $urlalise_object->dst;
    $url = url($url_alise, array('absolute' => TRUE));
    
    $document->newField('url', $url);
  }
  else {
    $url_with_lang = $resultdoc->nid;
    $url = url($url_with_lang, array('absolute' => TRUE));
    $document->newField('url', $url );
  }

  $document->newField('url', $url );
  $user_url = drupal_get_path_alias('user/' . $resultdoc->uid);
  $user=user_load($resultdoc->uid);
  $document->newField('timestamp', $resultdoc->timestamp);
  $document->newField('user_name', $user->name);
  $document->newField('user_email', $user->mail);
  $document->newField('user_url', url($user_url , array('absolute' => TRUE)));
  return $document;
}

/**
 * Generate documents for indexing.
 */

function generate_documents_to_index() {
  global $language;
  if (!isset($langcode)) {
    $langcode = $GLOBALS['language_content']->language;
  }
  $get_details = get_settings_query();
  $server_details = db_fetch_object($get_details);
  $types = get_drupal_types();
  while ($type = db_fetch_object($types)) {
    if ($type->enabled == 1) {
      $categories_check = get_drupal_categories();
      while ($cate_check = db_fetch_object($categories_check)) {
        if ($cate_check->enabled == 1) {
          $count_result_query="
            SELECT
            COUNT(*) FROM
            {node} n, {node_revisions} rev
            WHERE
            n.nid = rev.nid AND n.type LIKE '%s'";
          $count_result=db_query($count_result_query, $type->type);
          $result_count = db_result($count_result);
          for ($i = 0; $i < $result_count; $i++) {
            if ($i != 0) {
              $i = $j + 1;
            }
            $j = $i + 100;
            $sql=get_allpagest_db($server_details->date);
            $result = db_query($sql, $type->type, $i, $j);
            $index = new OssIndexDocument();
            while ($resultdoc = db_fetch_object($result)) {
              if ($resultdoc->status == 1) {
                $taxonomy = db_query('
                  SELECT *
                  FROM
                  {term_node}
                  WHERE
                  term_node.nid=%d', $resultdoc->nid
                );
                $taxonomyarray = db_fetch_object($taxonomy);
                $taxonomy_data = db_query('
                  SELECT * FROM
                  {term_data}
                  WHERE
                  term_data.tid=%d', $taxonomyarray->tid
                );
                $taxonomyarray_data = db_fetch_object($taxonomy_data);
                $document = $index->newDocument($language->language);
                create_index_document($document, $resultdoc, $taxonomyarray_data,$language->language);
                get_comment_documents($resultdoc);
              }
            }
            opensearchserver_start_indexing($index, $server_details);
          }
        }
      }
    }
  }
}

/**
 * Generate comment documents for indexing.
 */

function get_comment_documents($resultdoc) {
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
      opensearchserver_start_indexing($index, $server_details);
    }
  }
}
/**
 * Start indexing process
 */

function opensearchserver_start_indexing($index, $server_details) {
  $server = new OssApi($server_details->serverurl, $server_details->indexname);
  $server->credential($server_details->username, $server_details->key);
  if ($server->update($index, $server_details->indexname) === FALSE) {
    $errors[] = 'Failed to update';
  }
  $server->optimize();
}