<?php

/**
 * @file
 * Template file to show search result
 */
  print $opensearchserver_data['form'];
  if ($opensearchserver_data['q'] && isset($opensearchserver_data['result']) && $opensearchserver_data['result'] instanceof SimpleXMLElement) {
?>
  <div class="oss-results">
  <table border="0" width="100%">
  <tr>
  <?php
    if ($opensearchserver_data['oss_result']->getResultFound() > 0 && $opensearchserver_data['q'] != $opensearchserver_data['block_text']) {
    ?>
    <div align="left">
    <?php print check_plain($opensearchserver_data['oss_result']->getResultFound()); ?> documents found (<?php print check_plain($opensearchserver_data['result_time']);?> seconds)
    </div>
    <?php  $max = get_max($opensearchserver_data['oss_result']);
    }
    if ($opensearchserver_data['filter_result']) {
    ?>
      <td width="25%" valign="top">
      <div class="oss-facet">
      <div class="oss-facet-type"><?php print check_plain(t('Type'));?>
      <ul>
      <?php
      print $opensearchserver_data['print_facet_everything'];
      foreach ($opensearchserver_data['oss_result_facet']->getFacet('type') as $values) {
        ?>
        <li>
        <?php
        print check_same_filter($opensearchserver_data['fq'] , $values['name']) ? '<b>':'';
        ?>
        <a href="<?php print generate_filter_link($opensearchserver_data['q'] , '&fq=' , $values['name']);?>"> <?php print drupal_ucfirst(check_plain($values['name'])) . '(' .  $values . ')';?> </a>
        <?php
        print check_same_filter($opensearchserver_data['fq'] , $values['name']) ? '</b>':'';
        ?>
        </li>
        <?php
      }
      ?>
      </ul>
      </div>
      <br/>
      <div class="oss-facet-type">
      <?php print check_plain(t('Categories'));?>
      <ul>
      <?php
      print $opensearchserver_data['print_facet_everything'];
      foreach ($opensearchserver_data['oss_result_facet']->getFacet('taxonomy') as $categories) {
        ?>
        <li>
        <?php
        print check_same_filter($opensearchserver_data['tq'] , $categories['name']) ? '<b>':'';
        ?>
        <a href="<?php print generate_filter_link($opensearchserver_data['q'] , '&tq=' , $categories['name']);?>"> <?php print drupal_ucfirst(check_plain($categories['name'])) . '(' .  $categories . ')';?> </a>
        <?php
        print check_same_filter($opensearchserver_data['tq'] , $categories['name']) ? '</b>':'';
        ?>
        </li>
        <?php
      }
      ?>
      </ul>
      </div>
      <br/>
      <div class="oss-facet-type">
      <?php
      if ($opensearchserver_data['date_filter']) {
      ?>
        <?php print check_plain(t('Date'));?>
        <ul>
        <?php
        print $opensearchserver_data['print_facet_everything'];
        if ($opensearchserver_data['oss_result_facet']->getResultFound() > 0) {
          foreach ($opensearchserver_data['time_stamp'] as $time_stamp) {
            ?>
            <li>
            <?php
            print check_same_filter($opensearchserver_data['ts'] , $time_stamp) ? '<b>':'';
            ?>
            <a href="<?php print generate_filter_link($opensearchserver_data['q'] , '&ts=' , $time_stamp);?>"> <?php print drupal_ucfirst(check_plain($time_stamp));?> </a>
            <?php
            print check_same_filter($opensearchserver_data['ts'] , $time_stamp) ? '</b>' : '';
            ?>
            </li>
            <?php
          }
        }
      }
      ?>
      </ul>
      </div>
      <?php
      if ($opensearchserver_data['enable_language_filter']) {
        ?>
        <br/>
        <div class="oss-facet-type">
        <?php print check_plain(t('Language'));?>
        <ul>
        <?php
        foreach ($opensearchserver_data['oss_result_facet']->getFacet('lang') as $language) {
          ?>
          <li>
          <?php
          print check_same_filter($opensearchserver_data['lq'] , $language['name']) ? '<b>':'';
          ?>
          <a href="<?php print generate_filter_link($opensearchserver_data['q'] , '&lq=' , $language['name']);?>"> <?php print drupal_ucfirst(check_plain(get_local_facet($language['name']))) . '(' .  $language . ')';?> </a>
          <?php
          print check_same_filter($opensearchserver_data['lq'] , $language['name']) ? '</b>':'';
          ?>
          </li>
          <?php
        }
          ?>
          </ul>
          </div>
    <?php
        }
      ?>
    </div>
    </td>
    <?php
    }
    ?>
    <td width="75%" valign="top">
    <?php if ($opensearchserver_data['oss_result']->getResultFound() <= 0 || $search_query == $opensearchserver_data['block_text']) {?>
      <div align="left" class="oss-error">
      <?php if ($search_query == $opensearchserver_data['block_text']) { ?>
        <p>To be processed a query can't be empty and should contains valid words.</p>
      <?php } ?>
      <p> No documents containing all your search terms were found.</p>
      <p>Your Search Keyword <b><?php print check_plain(t($search_query)); ?></b> did not match any document</p><br/><p>Suggestions:</p>
      <p>- Make sure all wordsget_field are spelled correctly.</p>
      <p>- Try different keywords.</p>
      <p>- Try more general keywords.</p>
      </div>
    <?php
    }
    else {
      for ($i = $opensearchserver_data['oss_result']->getResultStart(); $i < $max; $i++) {
        $category = stripslashes($opensearchserver_data['oss_result']->getField($i, 'type', TRUE));
        if ($category=="comments") {
        ?>
        <div class="oss-result">
        <div class="oss-result-field1"><a href="<?php print check_url(stripslashes($opensearchserver_data['oss_result']->getField($i, 'url', TRUE)));?>"><?php print $subject;?></a> </div>
        <div class="oss-result-field2"><?php print stripslashes($opensearchserver_data['oss_result']->getField($i, 'comments_comment', TRUE));?></div>
        <?php
        if ($opensearchserver_data['signature'] == 1) {?>
          <div class="oss-result-field3"><a href="<?php print check_url(stripslashes($opensearchserver_data['oss_result']->getField($i, 'url', TRUE)));?>"><?php print stripslashes($opensearchserver_data['oss_result']->getField($i, 'url', TRUE));?></a>&nbsp;&nbsp;
          <?php  print check_plain(drupal_ucfirst($opensearchserver_data['oss_result']->getField($i, 'type', TRUE)));?> By <a href="<?php print stripslashes($opensearchserver_data['oss_result']->getField($i, 'user_url', TRUE));?>"><?php print stripslashes($opensearchserver_data['oss_result']->getField($i, 'user_name', TRUE));?></a>
          </div>
          <?php
        }
        else { ?>
          <div class="oss_result_field3"><a href="<?php print check_url(stripslashes($opensearchserver_data['oss_result']->getField($i, 'url', TRUE)))?>"><?php print check_url(stripslashes($opensearchserver_data['oss_result']->getField($i, 'url', TRUE)))?> </a></div><br/>
          <?php
        }
        ?>
        </div>
        <?php
        }
        else {
        ?>
        <div class="oss-result">
        <div class="oss-result-field1"><a href="<?php print check_url(stripslashes($opensearchserver_data['oss_result']->getField($i, 'url', TRUE)));?>"><?php print stripslashes($opensearchserver_data['oss_result']->getField($i, 'title', TRUE));?></a> </div>
        <div class="oss-result-field2"><?php print stripslashes($opensearchserver_data['oss_result']->getField($i, 'content', TRUE));?></div>
        <?php
        if ($opensearchserver_data['signature'] == 1) {
          ?>
          <div class="oss-result-field3"><a href="<?php print check_url(stripslashes($opensearchserver_data['oss_result']->getField($i, 'url', TRUE)))?>"><?php print opensearchserver_create_url_snippet($opensearchserver_data['oss_result']->getField($i, 'url', TRUE), $opensearchserver_data['url_snippet']);?> </a>&nbsp;&nbsp;
          <?php
          if ($opensearchserver_data['oss_result']->getField($i, 'user_name', TRUE) && $opensearchserver_data['oss_result']->getField($i, 'type', TRUE)) {
            print check_plain(drupal_ucfirst($opensearchserver_data['oss_result']->getField($i, 'type', TRUE)));?> By <a href="<?php print check_url(stripslashes($opensearchserver_data['oss_result']->getField($i, 'user_url', TRUE)));?>"><?php check_plain(print stripslashes($opensearchserver_data['oss_result']->getField($i, 'user_name', TRUE)));?></a>
            <?php
          }
          ?>
          </div>
          <?php
        }
        else {
        ?>
        <div class="oss-result-field3"><a href="<?php print check_url(stripslashes($opensearchserver_data['oss_result']->getField($i, 'url', TRUE)))?>"><?php print opensearchserver_create_url_snippet(stripslashes($opensearchserver_data['oss_result']->getField($i, 'url', TRUE)), $opensearchserver_data['url_snippet']);?> </a></div>
        <?php
        }
      }
      ?>
      </div>
      <?php
      }
      ?>
      <div class="oss-paging">
      <?php
      foreach ($opensearchserver_data['paging'] as $page) {?>
      <span class="<?php print $page['style']; ?>"> <a href="<?php print $page['url'];?>"><?php print $page['label']; ?></a></span>&nbsp;&nbsp;
      <?php
      }
      ?>
      </div>
      </div>
      <div align="right" class="oss_logo">
      <img src="http://www.open-search-server.com/images/oss_logo_62x60.png" /><br/>
      <a href="http://www.open-search-server.com/">Enterprise Search Made Yours</a>
      </div>
      <?php
    }
    ?>
    </td>
  </tr>
  </table>
      <?php
  }
  ?>
  </div>