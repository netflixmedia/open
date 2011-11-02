<?php

/**
 * @file
 * Class to access OpenSearchServer API
 */

if (!extension_loaded('SimpleXML')) {
  trigger_error("OssApi won't work whitout SimpleXML extension", E_USER_ERROR); die();
}

/**
 * Class to access OpenSearchServer API
 * @author philcube <egosse@open-search-server.com>
 * @package OpenSearchServer
 */
class OssPaging {

  protected $oss_result;
  protected $resultTotal;
  protected $resultLow;
  protected $resultHigh;
  protected $resultPrev;
  protected $resultNext;
  protected $pageBaseURI;
  protected $rowsParameter;
  protected $pageParameter;
  const MAX_PAGE_TO_LINK = 10;

  /**
   * @param $result The data
   * @param $model The list of fields
   * @return OssApi
   */
  public function __construct(SimpleXMLElement $result, $rowsParam = 'rows', $pageParam = 'p') {
    $this->oss_result  = $result;
    $this->rowsParameter = $rowsParam;
    $this->pageParameter = $pageParam;
    self::compute();

    if (!function_exists('OssApi_Dummy_Function')) {
      function OssApi_Dummy_Function() {
      }
    }
  }

  /**
   * GETTER
   */
  public function getResultCurrentPage() {
    return $this->resultCurrentPage;
  }

  public function getResultTotal() {
    return $this->resultTotal;
  }

  public function getResultLow() {
    return $this->resultLow;
  }

  public function getResultHigh() {
    return $this->resultHigh;
  }

  public function getResultPrev() {
    return $this->resultPrev;
  }

  public function getResultNext() {
    return $this->resultNext;
  }

  public function getPageBaseURI() {
    return $this->pageBaseURI;
  }


  public function compute() {
    $this->resultFound   = ((int) $this->oss_result->result['numFound'] - (int) $this->oss_result->result['collapsedDocCount']);
    $this->resultTime    = (float) $this->oss_result->result['time'] / 1000;
    $this->resultRows    = (int) $this->oss_result->result['rows'];
    $this->resultStart   = (int) $this->oss_result->result['start'];

    $this->resultCurrentPage = floor($this->resultStart / $this->resultRows);
    $this->resultTotal  = ceil($this->resultFound / $this->resultRows);

    if ($this->resultTotal > 1) {
      $low  = $this->resultCurrentPage - (OssPaging::MAX_PAGE_TO_LINK / 2);
      $high = $this->resultCurrentPage + (OssPaging::MAX_PAGE_TO_LINK / 2 - 1);
      if ($low < 0) {
        $high += $low * -1;
      }
      if ($high > $this->resultTotal) {
        $low -= $high - $this->resultTotal;
      }

      $this->resultLow  = max($low, 0);
      $this->resultHigh = min($this->resultTotal, $high);
      $this->resultPrev = max($this->resultCurrentPage - 1, 0);
      $this->resultNext = min($this->resultCurrentPage + 1, $this->resultTotal);
      $this->pageBaseURI = preg_replace('/&(?:' . $this->pageParameter . '|' . $this->rowsParameter . ')=[\d]+/', '', $_SERVER[request_uri()]) . '&' . $this->rowsParameter . '=' . $this->resultRows . '&' . $this->pageParameter . '=';
    }
  }

}