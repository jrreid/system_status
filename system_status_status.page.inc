<?php
/**
 * @file
 * Logic for system_status output
 */

/**
 * Return JSON formatted module information.
 */
function system_status_status_page() {
  $system_modules = system_list('module_enabled');

  // Needless initialisation, but hey.
  $res = array(
    "core" => array(),
    "contrib" => array(),
    "custom" => array(),
  );

  foreach ($system_modules as $module => $module_info) {
    $filename = $module_info->filename;

    // Match for custom modules.
    if (variable_get('system_status_do_match_custom', '0')) {
      $regex = variable_get('system_status_preg_match_custom', '{^sites\/([A-z,\.,\-]+)\/modules\/custom\/*}');
      if (preg_match($regex, $filename)) {
        $res['custom'][$module] = array("version" => $module_info->info['version']);
      }
    }
    else {
      $res['custom'] = "disabled";
    }

    // Match for contrib modules.
    if (variable_get('system_status_do_match_contrib', '1')) {
      if (variable_get('system_status_match_contrib_mode', 0) == 0) {
        $regex = '{^sites\/([A-z,\.,\-]+)\/modules\/*}';
      }
      elseif (variable_get('system_status_match_contrib_mode', 0) == 1) {
        $regex = '{^sites\/([A-z,\.,\-]+)\/modules\/contrib\/*}';
      }
      else {
        $regex = variable_get('system_status_preg_match_contrib', '{^sites\/([A-z,\.,\-]+)\/modules\/contrib\/*}');
      }

      if (preg_match($regex, $filename)) {
        $res['contrib'][$module] = array("version" => $module_info->info['version']);
      }
    }
    else {
      $res['contrib'] = "disabled";
    }

    // Core modules.
    if (variable_get('system_status_do_match_core', '1')) {
      if (strtolower($module_info->info['package']) == "core") {
        $res['core'][$module] = array("version" => $module_info->info['version']);
      }
    }
    else {
      $res['core'] = "disabled";
    }
  }

  drupal_json_output(array("system_status" => $res));
}