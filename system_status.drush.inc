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

function drush_system_status_drupalstatus_register($apikey='API-KEY-COMING-SOON') {

  $status_token = variable_get('system_status_token','Error-no-token');
  $encrypt_token = variable_get('system_status_encrypt_token', 'Error-no-token');

  global $base_url;

  $externalurl = drush_get_option('externalurl');

  if($externalurl) {
    $siteUrl = $externalurl;
  } else {
    $siteUrl = $base_url;
  }

  // Do an initial verification of the URL

  if(!filter_var($siteUrl, FILTER_VALIDATE_URL)) {
    drush_log("The url (".$siteUrl.") for this site does not appear to be valid", "error");
    return;
  }

  $siteUrl = urlencode($siteUrl);
  $siteUrl .= "|" . $status_token;
  $siteUrl .= "|" . $encrypt_token;

  $url = "https://www.drupalstatus.org/addSiteUsingKey?key=".$apikey."&siteUrl=" . $siteUrl;

  drush_log($url." is the URL","ok");
}
