<?php

/**
 * @file
 * Class to access OpenSearchServer API
 */

if (!class_exists('RuntimeException')) {
  class RuntimeException extends Exception {}
}
if (!class_exists('LogicException')) {
  class LogicException extends Exception {}
}
if (!class_exists('InvalidArgumentException')) {
  class InvalidArgumentException extends LogicException {}
}
if (!class_exists('OutOfRangeException')) {
  class OutOfRangeException extends LogicException {}
}

/**
 * Open Search Server Exception
 * @author pmercier <pmercier@open-search-server.com>
 * @package OpenSearchServer
 * FIXME Complete this documentation
 */
class OssException extends RuntimeException {

  private $status;
  protected $message;

  public function __construct($xml) {

    if ($xml instanceof SimpleXMLElement) {
      $xmlDoc = $xml;
    }
    elseif ($xml instanceof DOMDocument) {
      $xmlDoc = simplexml_import_dom($xml);
    }
    else {
      $previous_error_level = error_reporting(0);
      $xmlDoc = simplexml_load_string($xml);
      error_reporting($previous_error_level);

      if (!$xmlDoc) {
        throw new RuntimeException("The provided parameter is not a valid XML data. Please use OSSAPI::isOSSError before throwing this exception.");
      }
    }

    $data = array();
    foreach ($xmlDoc->entry as $entry)
    $data[(string)$entry['key']] = (string)$entry;

    $this->status  = $data['Status'];

    parent::__construct($data['Exception'], 0);

  }

  /**
   * Return the error status from the search engine
   * @return string
   */
  public function getStatus() {
    return $this->status;
  }

}

/**
 * Open Search Server Tomcat Exception
 * @author pmercier <pmercier@open-search-server.com>
 * @package OpenSearchServer
 * FIXME Complete this documentation
 */
class TomcatException extends RuntimeException {

  private   $status;
  protected $message;

  public function __construct($code, $html) {

    // Tomcat don't return a valid XHTML document, so we use preg_match
    $matches = array();
    if (!preg_match_all('/<p>(?:(?:(?!<\/p>).)*)<\/p>/mi', $html, $matches)) {
      $message = "Tomcat returned an unknown error.";
    }
    else {
      $message = strip_tags(end($matches[0]));
      $message = drupal_substr($message, strpos($message, ' '));
    }

    parent::__construct($message, (int)$code);

  }

}