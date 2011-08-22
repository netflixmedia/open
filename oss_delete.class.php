<?php

/**
 * @file
 * Class to access OpenSearchServer API
 */

if (!class_exists('OssApi')) {
  trigger_error("OssSearch won't work whitout OssApi", E_USER_ERROR); die();
}

class OssDelete {

  protected $enginePath;
  protected $index;

  public function __construct($enginePath, $index = NULL, $login = NULL, $apiKey = NULL) {
    $ossAPI = new OssApi($enginePath, $index);
    $this->enginePath  = $ossAPI->getEnginePath();
    $this->index    = $ossAPI->getIndex();
    $this->credential($login, $apiKey);
  }

  public function delete($query) {
    $params = array("q" => $query);
    $return = OssApi::queryServerXML($this->getQueryURL(OssApi::API_DELETE, $this->index  , OssApi::API_SCHEMA_DELETE_FIELD, $params));
    if ($return === FALSE) {
      return FALSE;
    }
    return TRUE;
  }

  public function credential($login, $apiKey) {
    // Remove credentials
    if (empty($login)) {
      $this->login  = NULL;
      $this->apiKey  = NULL;
      return;
    }

    // Else parse and affect new credentials
    if (empty($login) || empty($apiKey)) {
      if (class_exists('OssException')) {
        throw new UnexpectedValueException('You must provide a login and an api key to use credential.');
      }
      trigger_error(check_plain(__CLASS__ . '::' . __METHOD__ . ': You must provide a login and an api key to use credential.', E_USER_ERROR));
      return FALSE;
    }

    $this->login  = $login;
    $this->apiKey  = $apiKey;
  }

  protected function getQueryURL($apiCall, $index = NULL, $cmd = NULL, $options = NULL) {

    $path = $this->enginePath . '/' . $apiCall;
    $chunks = array();

    if (!empty($index)) {
      $chunks[] = 'use=' . urlencode($index);
    }

    if (!empty($cmd)) {
      $chunks[] = 'cmd=' . urlencode($cmd);
    }

    // If credential provided, include them in the query url
    if (!empty($this->login)) {
      $chunks[] = "login=" . urlencode($this->login);
      $chunks[] = "key="  . urlencode($this->apiKey);
    }

    // Prepare additionnal parameters
    if (is_array($options)) {
      foreach ($options as $argName => $argValue) {
        $chunks[] = $argName . "=" . urlencode($argValue);
      }
    }

    $path .= (strpos($path, '?') !== FALSE ? '&' : '?') . implode("&", $chunks);

    return $path;
  }
}