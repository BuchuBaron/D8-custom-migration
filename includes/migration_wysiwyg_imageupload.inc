<?php

/**
 * @file
 * Deal with wywiwyg_imageupload tags.
 */

/**
 * Replace a tag of form [[wysiwyg_imageupload:nn:]] with html and colorbox.
 */
function migration_html_from_wysiwyg_filter_iid($iid) {
  // First, get the fid and use it to find the image file.
  \Drupal\Core\Database\Database::setActiveConnection('legacy');
  $db = \Drupal\Core\Database\Database::getConnection();
  $query = $db->select('wysiwyg_imageupload_entity', 'wie')
    ->fields('wie', array('fid', 'nid', 'title'))
    ->fields('f', array('uid', 'filename', 'filepath'))
    ->condition('wie.iid', $iid);
  $query->join('files', 'f', 'f.fid = wie.fid');
  $result = $query->execute()
    ->fetchObject();
  \Drupal\Core\Database\Database::setActiveConnection();

  $imgPath = '/sites/capetrials.co.za/files/wiu/' .
    $result->uid . '/' . $result->filename; 
  $replacement =<<< EOS
  <a href="$imgPath" class="colorbox">
    <img src="$imgPath" align="center">
  </a>
EOS;
  return $replacement;
}

// vim: et sw=2 ft=php ai :
