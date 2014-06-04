<?php

function system_status_drush_command() {
  $items = array();

  $items['drupalstatus-register'] = array(
    'description' => "Register this site with drupalstatus.org",
    'arguments' => array(
      'apikey' => 'The drupalstatus.org API key for your account',
    ),
    'required-arguments' => TRUE,
    'options' => array(
      'externalurl' => 'The external reachable URL of this site (defaults to $base_url)',
      'https' => 'Force use of HTTPS',
    ),
    'examples' => array(
      'drush drupalstatus-register ACCOUNT-API-KEY' => 'Register this site with drupalstatus.org.',
      'drush drupalstatus-register ACCOUNT-API-KEY --externalurl=www.example.com' => 'Register this site as www.example.com with drupalstatus.org.',
    ),
    'aliases' => array('dsr'),
    'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_LOGIN,
  );

  return $items;
}

function drush_system_status_drupalstatus_register($apikey) {

  $status_token = variable_get('system_status_token','Error-no-token');
  $encrypt_token = variable_get('system_status_encrypt_token', 'Error-no-token');

  global $base_url;

  $externalurl = drush_get_option('externalurl');

  if($externalurl) {
    $siteUrl = $externalurl;
  } else {
    $siteUrl = $base_url;
  }

  // Check if force https
  if(drush_get_option('https')) {
    $siteUrl = str_replace('http:', 'https:', $siteUrl);

    // Check that HTTPS is the same site
    drush_print("Checking HTTPS at ".$siteUrl."/admin/reports/system_status/".$status_token);
    #$result = drupal_http_request($siteUrl."/admin/reports/system_status/".$status_token);
    #var_dump($result);
    #var_dump($siteUrl."/admin/reports/system_status/".$status_token);
  }

  // Do an initial validation of the URL
  if(!filter_var($siteUrl, FILTER_VALIDATE_URL)) {
    drush_log("The url (".$siteUrl.") for this site does not appear to be valid", "error");
    return;
  }

  drush_log("Submitting ".$siteUrl." to drupalstatus.org", "ok");

  // Assemble the URL and token values
  $siteUrl = urlencode($siteUrl);
  $siteUrl .= "|" . $status_token;
  $siteUrl .= "|" . $encrypt_token;

  $fields = "key=".$apikey."&siteUrl=" . $siteUrl;

  // make request from drupalstatus.org API via curl
  if(function_exists('curl_version')) {
    $ch = curl_init();
    curl_setopt($ch, CURLOPT_URL, "https://drupalstatus.org/api/addSiteUsingKey");
    curl_setopt($ch, CURLOPT_POST, 1);
    curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
    curl_setopt($ch, CURLOPT_POSTFIELDS, $fields);
    $curl_result = curl_exec($ch);
    $info = curl_getinfo($ch);
    curl_close($ch);
  } else {
    drush_log("CURL support must be present", "error");
    return;
  }

  $result = json_decode($curl_result);

  // check the request
  if($result->status == "ok") { 
    drush_log("This site has been added to drupalstatus.org and will appear shortly", "ok");
  } else {
    drush_log("This site could not be added to drupalstatus.org. Please verify the URL of the site is correct", "error");
  }
}
