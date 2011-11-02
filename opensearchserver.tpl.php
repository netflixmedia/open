<?php

/**
 * @file
 * Template file to show search result
 */

print $opensearchserver_data['form'];?>
<div id="results">
<?php
  if (isset($opensearchserver_data['result']) && $opensearchserver_data['result'] instanceof SimpleXMLElement) {
    $signature = db_query("
    SELECT
    signature,
    filter_enabled,
    url_snippet,
    date_filter
    FROM
    {opensearchserver}"
    );
    $signaturedetails = db_fetch_object($signature);
    $oss_result = new OssResults($opensearchserver_data['result'], NULL);
    if ($oss_result->getResultFound() <= 0 ||  $opensearchserver_data['q'] == $opensearchserver_data['block_text']) {
      ?>
<table width="100%" border="0">
  <tr>
	<?php if ($opensearchserver_data['no_filter'] == 1) {?>
     <td width="20%">
 <div align="left" style="margin-top: -20px;">
   <div class="oss_facet">Type <br/>
     <div class="oss_facet_type">
     <ul>
       <li>
         <div class="oss_facet_all">
          <a href="/?q=opensearchserver/search/<?php check_plain(check_plain(drupal_urlencode(print $opensearchserver_data['q'])));?>">Everything</a>
          </div>
       </li>
     </ul>
     </div>
     <div class="oss_facet_categories"><br/>
     Categories
     <ul>
      <li>
       <div class="oss_facet_all">
         <a href="/?q=opensearchserver/search/<?php  check_plain(drupal_urlencode(print $opensearchserver_data['q'])); ?>">Everything</a>
        </div>
        </li>
        </ul>
     </div>
     <div class="oss_facet_categories"><br/>
     Date
     <ul><li><div class="oss_facet_all">   <a href="/?q=opensearchserver/search/<?php  check_plain(drupal_urlencode(print $opensearchserver_data['q'])); ?>">Any Time</a></div></li>
     </ul>
     </div>
     </div>
     </div>
       </td>
<? }?>
    <td width="80%">
      <div align="left" style="margin-top: 10px;">
			<?php if ($opensearchserver_data['q'] == $opensearchserver_data['block_text']) { ?>
			<p>To be processed a query can't be empty and should contains valid words.</p>
			<?php }?>
			<p> No documents containing all your search terms were found.</p>
			<p>Your Search Keyword <b><?php print $opensearchserver_data['q']; ?></b> did not match any document</p><br/><p>Suggestions:</p>
			 <p>- Make sure all words are spelled correctly.</p>
			 <p>- Try different keywords.</p>
             <p>- Try more general keywords.</p>
	  </div>
	</td>
  </tr>
 </table>
</div>
<?php
  }
  else {
  $result_time = (float)$opensearchserver_data['result']->result['time'] / 1000;?>
  <div align="left"><?php print check_plain($oss_result->getResultFound()); ?> documents found (<?php print check_plain($result_time);?> seconds)</div>
  <?php  $max = ($oss_result->getResultStart() + $oss_result->getResultRows() > $oss_result->getResultFound()) ? $oss_result->getResultFound() : $oss_result->getResultStart() + $oss_result->getResultRows();?>
  <table width="100%" border="0">
  <tr>
  <?php
  if ($signaturedetails->filter_enabled) {
  ?>
  <td width="20%">
  <div class="oss_facet">Type
  <div class="oss_facet_type">
  <ul>
  <li>
  <div class="oss_facet_all">
  <a href="/?q=opensearchserver/search/<?php check_plain(drupal_urlencode(print $opensearchserver_data['q']));?>">Everything</a><br/></div></li>
  <?php
  foreach ($oss_result->getFacet('type') as $values) {
    $value = $values['name'];
    ?>
   <li> <a href="/?q=opensearchserver/search/<?php print drupal_urlencode($opensearchserver_data['q']);?>/&fq=<?php print drupal_urlencode($value);?>"> <?php print drupal_ucfirst(check_plain($value)) . '(' .  $values . ')';?> </a></li>
    <?php
  }
  ?>
   </ul>
  </div>
  <?php
  if ($oss_result->getFacet('taxonomy')) {
  ?>
  <div class="oss_facet_categories"><br/>
  Categories
  <ul>
  <li>
  <div class="oss_facet_all">
  <a href="/?q=opensearchserver/search/<?php  check_plain(drupal_urlencode(print $opensearchserver_data['q'])); ?>">Everything</a>
  </div>
  </li>
   <?php
  foreach ($oss_result->getFacet('taxonomy') as $taxonomys) {

  $taxonomy_name = $taxonomys['name'];
    ?>
   <li> <a href="/?q=opensearchserver/search/<?php print urlencode($opensearchserver_data['q']);?>/&tq=<?php print drupal_urlencode($taxonomy_name);?>"> <?php print drupal_ucfirst(check_plain($taxonomy_name)) . '(' .  $taxonomys . ')';?> </a></li>
    <?php
  }
}
  ?>
  </ul>
  </div>
  <?php   if ($signaturedetails->date_filter) {
  ?>
  <div class="oss_facet_categories"><br/>
  Date
  <ul><li><div class="oss_facet_all">   <a href="/?q=opensearchserver/search/<?php  check_plain(drupal_urlencode(print $opensearchserver_data['q'])); ?>">Any Time</a></div></li>
     <?php
  foreach ($opensearchserver_data['time_stamp'] as $timestamp) {
    ?>
   <li> <a href="/?q=opensearchserver/search/<?php print drupal_urlencode($opensearchserver_data['q']);?>/&ts=<?php print drupal_urlencode($timestamp);?>"> <?php print drupal_ucfirst(check_plain($timestamp));?> </a></li>
    <?php
  }
  ?>
  </ul>
  </div>
  </div>
  </td>
  <?php }}?>
  <td width="80%">
  <div class="oss_results">
  <?php
  for ($i = $oss_result->getResultStart(); $i < $max; $i++) {
    $category = stripslashes($oss_result->getField($i, 'type', TRUE));
    if ($category=="comments") {
      $subject = stripslashes($oss_result->getField($i, 'comments_subject', TRUE));
      $comment = stripslashes($oss_result->getField($i, 'comments_comment', TRUE));
      $user = stripslashes($oss_result->getField($i, 'user_name', TRUE));
      $user_url = stripslashes($oss_result->getField($i, 'user_url', TRUE));
      $type = stripslashes($oss_result->getField($i, 'type', TRUE));
      $url = stripslashes($oss_result->getField($i, 'url', TRUE));
?>
<div class="oss_result_field1"><a href="<?php print check_url($url);?>"><?php print $subject;?></a> </div>
<div class="oss_result_field2"><?php print $comment;?></div>
<?php
    if ($signaturedetails->signature==1) {?>
      <div class="oss_result_field3"><a href="<?php print check_url($url);?>"><?php print $comment;?></a>&nbsp;&nbsp;&nbsp;&nbsp;
	<?php print $type;?> By <a href="<?php print $user_url;?>"><?php print $user;?></a>
    </div>
  <?php
  }
  else {?>
    <div class="oss_result_field3"><a href="<?php print check_url($url)?>"><?php print check_url($url)?> </a></div><br/>
  <?php }
 ?>

<?php
    }
    else {
  $title = stripslashes($oss_result->getField($i, 'title', TRUE));
  $content = stripslashes($oss_result->getField($i, 'content', TRUE));
  $user = stripslashes($oss_result->getField($i, 'user_name', TRUE));
  $user_url = stripslashes($oss_result->getField($i, 'user_url', TRUE));
  $type = stripslashes($oss_result->getField($i, 'type', TRUE));
  $url = stripslashes($oss_result->getField($i, 'url', TRUE));
?>
      <div class="oss_result_field1"><a href="<?php print check_url($url);?>"><?php print $title;?></a> </div>
      <div class="oss_result_field2"><?php print $content;?></div>
      <?php
          if ($signaturedetails->signature==1) {?>
            <div class="oss_result_field3"><a href="<?php print check_url($url)?>"><?php print opensearchserver_create_url_snippet($url, $signaturedetails->url_snippet);?> </a>&nbsp;&nbsp;&nbsp;&nbsp;
      	<?php print check_plain($type);?> By <a href="<?php print check_url($user_url);?>"><?php check_plain(print $user);?></a>
          </div>
        <?php
        }
        else {?>
            <div class="oss_result_field3"><a href="<?php print check_url($url)?>"><?php print opensearchserver_create_url_snippet($url, $signaturedetails->url_snippet);?> </a></div>
          <?php }
    }
  }
  ?>
  <br/>
  <?php
  foreach ($opensearchserver_data['paging'] as $page) {?>
    <span class="<?php print $page['style']; ?>"> <a href="<?php print  check_url($opensearchserver_data['base_url']) . '/opensearchserver/search/' . $opensearchserver_data['q'] . $page['url'];?>"><?php print $page['label']; ?></a></span>&nbsp;&nbsp;&nbsp;
  <?php }
  ?>
  </div>
 <div align="right" class="oss_logo">
 <img src="http://www.open-search-server.com/images/oss_logo_62x60.png" /><br/>
 <a href="http://www.open-search-server.com/">Enterprise Search Made Yours</a>
  </div>
  	</td>
    </tr>
   </table>
  <?php
  }
}

