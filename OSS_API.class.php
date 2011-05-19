<?php
require 'OSS_API.class.php';
require 'misc.lib.php';
require 'OSS_IndexDocument.class.php';
require 'OSS_Results.class.php';
require 'OSS_Paging.class.php';
require 'OSS_Search.class.php';
require 'OSS_SearchTemplate.class.php';
 function opensearchserver_perm() {
  return array('access opensearchserver');
}

function opensearchserver_menu() {
  $items = array();
 $items['admin/settings/opensearchserver'] = array(
    'title'              => 'OpenSearchServer Server Settings',
    'description'        => 'Settings For OpenSearchServer Index path,Search Settings etc.',
    'page callback'      => 'admin_Settings',
    'page arguments'     => array('opensearchserver_settings'),
    'access callback'    => 'user_access',
    'access arguments'   =>  array('access opensearchserver'),
    
  );
   
  $items['opensearchserver'] = array(
    'title' => '',
    'description' => 'This module will integrate OpenSearchServer 1.2 as search engine for Drupal 6.x',
    'page callback' => 'opensearchserver_all',
    'access arguments' => array('access opensearchserver'),
    'type' => MENU_NORMAL_ITEM
  );
  return $items;
}
 
function admin_Settings() {
 
 return drupal_get_form('admin_index_form');
 
}
function opensearchserver_preprocess_search_theme_form(&$vars, $hook)
{
	  $vars['form']['search_theme_form']['#value'] = t('Search this Site');

}
function opensearchserver_form_alter(&$form, $form_state, $form_id) {
    $form_id_processed = $form_id;
	$arg = arg(NULL, $_GET['q']);
	if($arg[2]!=null)
	{
		$value=$arg[2];
	}
	else
	{
		$value='';
	}
   switch ($form_id_processed) {

      case 'search_form':
  
				 $form['basic']['inline']['keys'] = array(
				'#type' => 'textfield',
				'#title' => '',
				'#default_value'  => $value,
				'#maxlength' => 255);
				
				$form['basic']['inline']['submit'] = array('#type' => 'submit', '#value' => t('Search'));
				$form['#validate'][] = 'opensearchserver_validate';
				$form['#submit'][] = 'opensearchserver_submit';

				break;
	  case 'search_theme_form':
      
        			 $form['search_theme_form'] = array(
					'#type' => 'textfield',
					'#title' => '',
					'#size' => 15,
					'#maxlength' => 255,
				  );
				    $form['submit'] = array('#type' => 'submit', '#value' => t('Search'));
				  	$form['#submit'][] = 'opensearchserver_submit';
				  break;
		}
   
}
function opensearchserver_validate($form, &$form_state) {

  if (empty($form_state['values']['keys'])) {
    form_set_error('keywords', t('Please enter An Search Term.'));
  }
}
function opensearchserver_submit($form, &$form_state) {

	if($form_state['values']['search_theme_form'])
	{
		$form_state['redirect'] = 'opensearchserver/search/'. $form_state['values']['search_theme_form'];
	}
	else
	{
		$form_state['redirect'] = 'opensearchserver/search/'. $form_state['values']['keys'];
	}
    
}
function admin_index_form()
{
	 $details = db_query("SELECT * FROM {opensearchserver}");
	$sdetails = db_fetch_object($details);
	$form['serverurl'] = array(
     '#type' => 'textfield',
 	 '#title' => t('Enter the OpenSearchServer URL'),
 	 '#default_value' => $sdetails->serverurl,
	 
   );
   $form['indexname'] = array(
     '#type' => 'textfield',
 	 '#title' => t('Enter the Indexname URL'),
 	 '#default_value' => $sdetails->indexname,
   );
   $form['username'] = array(
     '#type' => 'textfield',
 	 '#title' => t('Enter the OpenSearchServer UserName for authentication'),
 	 '#default_value' => $sdetails->username,
   );
   $form['key'] = array(
     '#type' => 'textfield',
 	 '#title' => t('Enter the OpenSearchServer key for authentication'),
 	 '#default_value' => $sdetails->key,
   );
 
  $form['submit'] = array('#type' => 'submit', '#value' => t('Save'),'#name'=>'save',);
  $form['StartIndexing'] = array('#type' => 'submit', '#value' => t('Start Indexing'),'#name'=>'index');
	return $form;
}
function admin_index_form_validate($form_id, &$form_state)
{
	 if($form_state['clicked_button']['#name']=='save') {
    
		 if (empty($form_state['values']['indexname'])) {
			form_set_error('keywords', t('Please enter the Index Name.'));
		  }
		   if (empty($form_state['values']['serverurl'])) {
			form_set_error('keywords', t('Please enter the Serverurl'));
		  }
		
	}
}
function configure_OSS($url,$indexname,$username,$key)
{
			  $ossAPI = new OSS_API($url);
			  $ossAPI->credential($username,$key);
			  $ossAPI->createIndex($indexname);
			  $ossAPI->optimize($indexname);
			  $ossAPI->reload($indexname);
	return true;
}
function setFields_OSS($url,$indexname,$username,$key)
{
			  
			  $ossAPI = new OSS_API($url,$indexname);
			  $ossAPI->credential($username,$key);
			  $ossAPI->setField('id','','NO','YES','YES','','NO','YES');
			  $ossAPI->setField('category','','NO','YES','YES','','NO','NO');
			  $ossAPI->setField('pages_url','','NO','YES','YES','','NO','NO');
			  $ossAPI->setField('pages_title','TextAnalyzer','compress','YES','positions_offsets','','NO','NO');
			  $ossAPI->setField('pages_content','TextAnalyzer','compress','YES','positions_offsets','','YES','NO');
			  $ossAPI->setField('comments_url','','NO','YES','YES','','NO','NO');
			  $ossAPI->setField('comments_subject','TextAnalyzer','compress','YES','positions_offsets','','NO','NO');
			  $ossAPI->setField('comments_comment','TextAnalyzer','compress','YES','positions_offsets','','NO','NO');
			  $ossAPI->setField('users_url','','NO','YES','YES','','NO','NO');
			  $ossAPI->setField('users_name','TextAnalyzer','compress','YES','positions_offsets','','NO','NO');
			  $ossAPI->setField('users_Email','TextAnalyzer','compress','YES','positions_offsets','','NO','NO');
	  
			  $searchTemplate=new OSS_SearchTemplate($url,$indexname);
			  $searchTemplate->credential($username,$key);
			  $searchTemplate->createSearchTemplate("search",'
			  comments_subject:($$)^10 OR comments_subject:("$$")^10
					OR
			comments_comment:($$)^10 OR comments_comment:("$$")^10
			OR comments_url:($$)^5 OR
			 comments_url:("$$")^5
			OR
			 users_name:($$)^10 OR users_name:("$$")^10
					OR
			users_email:($$)^10 OR users_email:("$$")^10
			OR users_url:($$)^5 OR
			 users_url:("$$")^5
			OR
			 pages_title:($$)^10 OR pages_title:("$$")^10
					OR
			pages_content:($$)^10 OR pages_content:("$$")^10
			OR pages_url:($$)^5 OR
			 pages_url:("$$")^5
			
			 ',"AND","10","2","ENGLISH");
			 
			   $searchTemplate->setReturnField("search","category");
			   $searchTemplate->setReturnField("search","pages_url");
			   $searchTemplate->setReturnField("search","comments_url");
			   $searchTemplate->setReturnField("search","users_url");
			   $searchTemplate->setSnippetField("search","comments_comment");
			   $searchTemplate->setSnippetField("search","comments_subject");
			   $searchTemplate->setSnippetField("search","pages_title");
			   $searchTemplate->setSnippetField("search","pages_content");
			   $searchTemplate->setSnippetField("search","users_name");
			   $searchTemplate->setSnippetField("search","users_email");
			     $ossAPI->optimize($indexname);
			  $ossAPI->reload($indexname);
}
function admin_index_form_submit($form_id, &$form_state)
{
 switch ($form_state['clicked_button']['#name']) {
    case 'save':
		$result = db_query("SELECT * FROM {opensearchserver}");
		$count = $result->num_rows;
		if($count==0)
		{
			$iname=$form_state['values']['indexname'];
			if(configure_OSS($form_state['values']['serverurl'],$iname,$form_state['values']['username'], $form_state['values']['key']))
			{
				setFields_OSS($form_state['values']['serverurl'],$iname,$form_state['values']['username'], $form_state['values']['key']);
				db_query("INSERT INTO `{opensearchserver}` (`serverurl`, `indexname`, `username`, `key`) VALUES ('%s', '%s', '%s', '%s')", $form_state['values']['serverurl'], $form_state['values']['indexname'], $form_state['values']['username'], $form_state['values']['key']);
			}
			else
			{
				drupal_set_message(t('UnExpected Error Occurred'));
			}
			
		}
		else
		{
			db_query("UPDATE `{opensearchserver}` SET `serverurl` = '%s',`indexname`='%s',`username`='%s',`key`='%s'",$form_state['values']['serverurl'],$form_state['values']['indexname'],$form_state['values']['username'],$form_state['values']['key']);
		}
		drupal_set_message(t('The Preference has been saved'));
		break;
	  case 'index':
	 
			   $batch = array ('operations' => array (),
				'finished' => 'get_content_index_finished',
				'title' => t('Processing'),
				'init_message' => t('Starting ...'),
				 'error_message' => t('An error occurred and some or all of the exports have failed.'),
				);
					$batch ['operations'] [] = array ('get_content_index', array ($node));

					batch_set ($batch);
					batch_process ('admin/settings/opensearchserver'); 
				  
			break;
		
	 }
	 
}
function get_content_index_finished()
{
drupal_set_message(t('Reindexed all records.'));
}
function get_content_index()
{
	 
	global $base_url,$url,$uurl,$curl;
	global $base_root;
	$path=parse_url($base_root . request_uri());
		if($path[query])
		{
			$url=$base_url.'/?q=node/';
			$uurl=$base_url.'/?q=user/';
			 
		}
		else
		{
			$url=$base_url.'/node/';
			$uurl=$base_url.'/user/';
		}
		 
  	$result = db_query("SELECT * FROM {node_revisions}");
	$resultUser = db_query("SELECT * FROM {users}");
	$resultComment= db_query("SELECT * FROM {comments}");
	$getDetails = db_query("SELECT * FROM {opensearchserver}");
	$serverDetails = db_fetch_object($getDetails);
	
	$ossEnginePath  = configRequestValue('ossEnginePath', $serverDetails->serverurl, 'engineURL');
	$ossEngineConnectTimeOut = configRequestValue('ossEngineConnectTimeOut', 5, 'engineConnectTimeOut');
	$ossEngineIndex = configRequestValue('ossEngineIndex', $serverDetails->indexname, 'engineIndex');
	
		 $index = new OSS_IndexDocument();
	 	 while ($user = db_fetch_object($resultUser)) {
		 	$document = $index->newDocument('en');
			$document->newField('id', 'users_'.$user->uid);
			$document->newField('category', 'users');
			$document->newField('users_url',$uurl.$user->uid);
			$document->newField('users_name',$user->name);
			$document->newField('users_mail',$user->mail);
			$server = new OSS_API($ossEnginePath, $ossEngineIndex);
			$server->credential($serverDetails->username, $serverDetails->key);
		if ($server->update($index,$ossEngineIndex) === false) {
			$errors[] = 'failedToUpdate';
		 }
		}
		  $index = new OSS_IndexDocument();
	 	 while ($comment = db_fetch_object($resultComment)) {
		
			$document = $index->newDocument('en');
		   	$document->newField('id', 'comments_'.$comment->cid);
			$document->newField('category', 'comments');
			$document->newField('comments_url',	 $url.$comment->nid.'#comment-'.$comment->cid);
			$document->newField('comments_subject',	 $comment->subject);
			$document->newField('comments_comment',		 $comment->comment);
			 $server = new OSS_API($ossEnginePath, $ossEngineIndex);
			 		$server->credential($serverDetails->username, $serverDetails->key);
		if ($server->update($index,$ossEngineIndex) === false) {
			$errors[] = 'failedToUpdate';
		 }
		} 
	  	 $index = new OSS_IndexDocument();
		 while ($link = db_fetch_object($result)) {
		    $document = $index->newDocument('en');
			$document->newField('id', 'pages_'.$link->nid);
			$document->newField('category', 'pages');
			$document->newField('pages_url',$url.$link->nid);
			$document->newField('pages_title',$link->title);
			$document->newField('pages_content',$link->body);
			 $server = new OSS_API($ossEnginePath, $ossEngineIndex);
			 		$server->credential($serverDetails->username, $serverDetails->key);
		if ($server->update($index,$ossEngineIndex) === false) {
			$errors[] = 'failedToUpdate';
		 }
		} 
}
function opensearchserver_all() {
	$arg = arg(NULL, $_GET['q']);
	$result=getSearchResult($arg[2]);
	 
	$cont=drupal_get_form('opensearchserver_page_form');
	  		if (isset($result) && $result instanceof SimpleXMLElement) {

		$ossResults = new OSS_Results($result);
		$resultTime = (float)$result->result['time'] / 1000;
		$cont.=$ossResults->getResultFound().' documents found ('.$resultTime.' seconds)';
		  
		$max = ($ossResults->getResultStart() + $ossResults->getResultRows() > $ossResults->getResultFound()) ? $ossResults->getResultFound() : $ossResults->getResultStart() + $ossResults->getResultRows();
		//Documents iteration
		for ($i = $ossResults->getResultStart(); $i < $max; $i++) {
			$category	 = stripslashes($ossResults->getField($i, 'category', true));
			if($category=="pages")
			{
				$title	 = stripslashes($ossResults->getField($i, 'pages_title', true));
				$content = stripslashes($ossResults->getField($i, 'pages_content', true));
				$url = stripslashes($ossResults->getField($i, 'pages_url', false));
				$cont.='<p>';
				$cont.='<a href="'.$url.'">'.$title.'</a><br/>';
				$cont.=$content.'<br/>';
				$cont.='<a href='.$url.'>'.$url.'</a><br/>';
				$cont.='</p>';
			}
			else if($category=="users")
			{
				$name = stripslashes($ossResults->getField($i, 'users_name', true));
				$email = stripslashes($ossResults->getField($i, 'users_email', true));
				$url = stripslashes($ossResults->getField($i, 'users_url', false));
				$cont.='<p>';
				$cont.='<a href="'.$url.'">'.$name.'</a><br/>';
				$cont.=$email.'<br/>';
				$cont.='<a href='.$url.'>'.$url.'</a><br/>';
				$cont.='</p>';
			}
			else if($category=="comments")
			{
				$subject = stripslashes($ossResults->getField($i, 'comments_subject', true));
				$comment = stripslashes($ossResults->getField($i, 'comments_comment', true));
				$url = stripslashes($ossResults->getField($i, 'comments_url', false));
				$cont.='<p>';
				$cont.='<a href="'.$url.'">'.$subject.'</a><br/>';
				$cont.=$comment.'<br/>';
				$cont.='<a href='.$url.'>'.$url.'</a><br/>';
				$cont.='</p>';
			}
			
		
		}
			$cont.='<div align="right">';
			$cont.='<img src="http://www.open-search-server.com/images/oss_logo_62x60.png" /><br/>';
			$cont.='<a href="http://www.open-search-server.com/">Enterprise Search Made Yours</a>';
			$cont.='</div>';
			$ossPaging = new OSS_Paging($result, 'r', 'p');
			$pagingArray = array();
			if (isset($ossPaging) && $ossPaging->getResultTotal() >= 1) {
			print_r($ossPaging->getResultLow());
			   if ($ossPaging->getResultLow() > 0) {
				$label = 'First';
				$url = $ossPaging->getPageBaseURI().'1';
				$pagingArray[] = array('label' => $label, 'url' => $url);
			   }
			   if ($ossPaging->getResultPrev() < $ossPaging->getResultCurrentPage()) {
				$label = 'Previous';
				$url = $ossPaging->getPageBaseURI().($ossPaging->getResultPrev() + 1);
				$pagingArray[] = array('label' => $label, 'url' => $url);
			   }
			   for ($i = $ossPaging->getResultLow(); $i < $ossPaging->getResultHigh(); $i++) {
				if ($i == $ossPaging->getResultCurrentPage()) {
				 $label = $i + 1;
				 $url = null;
				} else {
				 $label = $i + 1;
				 $url = $ossPaging->getPageBaseURI().$label;
				 }
				$pagingArray[] = array('label' => $label, 'url' => $url);
				
			   }
			   if ($ossPaging->getResultNext() > $ossPaging->getResultCurrentPage()) {
				$label = 'Next';
				$url = $ossPaging->getPageBaseURI().($ossPaging->getResultNext() + 1);
				$pagingArray[] = array('label' => $label, 'url' => $url);

			   }
			  }
			  
			  foreach($pagingArray as $page)
			  {
				$cont.='<a href='.$page['url'].'>'.$page['label'].'</a>'.'&nbsp;&nbsp;&nbsp;';
			  }
		}
		
 return $cont;
}
 
 function getSearchResult($query)
 {
	 if($query)
	 {
			$getDetails = db_query("SELECT * FROM {opensearchserver}");
			$serverDetails = db_fetch_object($getDetails);
			$start = isset($_REQUEST['p']) ? $_REQUEST['p'] : null;
			$start = isset($start) ? max(0, $start - 1) * 10 : 0;
			$escapechars = array('\\', '^', '~', ':', '(', ')', '{', '}', '[', ']' , '&&', '||', '!', '*', '?');
			foreach ($escapechars as $escchar) $query = str_replace($escchar, ' ', $query);
			$query = trim($query);
			$search = new OSS_Search($serverDetails->serverurl, $serverDetails->indexname, 10, $start);
			$search->credential($serverDetails->username, $serverDetails->key);
			$result = $search->query($query)->template('search')->execute(5);
			
		
	}
	return $result;
 }
function opensearchserver_page_form() {
  $arg = arg(NULL, $_GET['q']);
	if($arg[2]!=null)
	{
		$value=$arg[2];
	}
	else
	{
		$value='';
	}
   $form['basic']['inline'] = array('#prefix' => '<div class="container-inline">', '#suffix' => '</div>');
   $form['basic']['inline']['q'] = array(
    '#type' => 'textfield',
    '#title' => '',
	'#default_value'=>$value,
    '#maxlength' => 255,
  );
 
  
  $form['basic']['inline']['submit'] = array('#type' => 'submit', '#value' => t('Search'));

  return $form;
}

function opensearchserver_page_form_submit($form_id, &$form_state) {
  
   $form_state['redirect'] = 'opensearchserver/search/'. $form_state['values']['q'];
}
 
function opensearchserver_block($op = 'list', $delta = 0, $edit = array()) {
  if ($op == "list") {
    $block = array();
    $block[0]["info"] = t('OpenSearchServer');
    return $block;
  }
  elseif ($op == 'view') {
    $block_content = 'OpenSearchServer';
    $block['subject'] = 'OpenSearchServer';
    $block['content'] = $block_content;
    return $block;
  }
}
