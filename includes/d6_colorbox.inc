<?php

/**
 * @file
 *   Code to replace image tags using lightbox with colorbox in the node body.
 *
 * Problem being solved: D6 site used lightbox for image uploads. The image
 * tags are not separate fields but are inline in the body.
 * These need to be decorated to work with colorbox since lightbox has no D8
 * version.
 */

use Drupal\migrate\Row;
use Drupal\migrate\Plugin\MigrateSourceInterface;
use Drupal\migrate\Plugin\MigrationInterface;

function capetrials_custom_migration_colorbox(Row $row, MigrateSourceInterface $source, MigrationInterface $migration) {

  // List fields that use long text fields with text formats.
  $text_fields = array(
    'body',
  );

  // Update nodes.
  $len = strlen('node') + strlen(CUSTOM_MIGRATION_PREFIX);
  drush_print_r($migration->id());
  if (substr($migration->id(), 0, $len) == CUSTOM_MIGRATION_PREFIX . 'node') {
    foreach ($text_fields as $text_field) {
      $value = $row->getSourceProperty($text_field);
      if (!empty($value)) {
        if (preg_match('/plugins\/emotions\/img/', $value)) {
          drush_print_r("*****>>>>> value=");
          // Replace "sites/all/libraries/tinymce/jscripts/tiny_mce/plugins/emotions/img/smiley-wink.gif" with public://img/
          $value = preg_replace(
            array(
              '#src="http://capetrials.co.za/sites/all/libraries/tinymce/jscripts/tiny_mce/plugins/emotions/img/#',
              '#src="/sites/all/libraries/tinymce/jscripts/tiny_mce/plugins/emotions/img/#',
            ),
            'src="/sites/capetrials.co.za/files/img/', 
            $value
          );
        }

        // <a href="http://capetrials.co.za/sites/capetrials.co.za/files/imagecache/wysiwyg_imageupload_lightbox_preset/wysiwyg_imageupload/7/IMG-20120909-00084_1.jpg" rel="lightbox[wysiwyg_imageupload_inline]" class="lightbox-processed">[[wysiwyg_imageupload:147:]]</a>
        // [[wysiwyg_imageupload:37:]]
        // in wysiwyg_imageupload_entity with iid=37: fid=74, title='blah blah'
        // in files with fid=74: uid=1, filename=rock.jpg =>
        //   path=files/wysiwyg_imageupload/1/rock.jpg, or files/ckutils/rock.jpg
        //
        // Replace with:
        // <a href="/sites/capetrials.co.za/files/ckutils/rock.jpg" class="colorbox">
        //   <img src="/sites/capetrials.co.za/files/ckutils/rock.jpg" title="$title" align="center" />
        // </a>

        while (preg_match('/\<a href="([^"]*)" rel="lightbox[^\>]*\>\[\[wysiwyg_imageupload:([0-9]*):\]\]\<\/a\>/', $value, $args)) {
          drush_print_r($row);
          // First, get the fid and use it to find the image file.
          \Drupal\Core\Database\Database::setActiveConnection('legacy');
          $db = \Drupal\Core\Database\Database::getConnection();
          $query = $db->select('wysiwyg_imageupload_entity', 'wie')
            ->fields('wie', array('fid', 'nid', 'title'))
            ->fields('f', array('uid', 'filename', 'filepath'))
            ->condition('wie.iid', $args[2]);
          $query->join('files', 'f', 'f.fid = wie.fid');
          $result = $query->execute()
            ->fetchObject();
          drush_print_r($result);
          drush_print_r($args);
          $imgPath = '/sites/capetrials.co.za/files/wiu/' .
            $result->uid . '/' . $result->filename; 
          $replacement =<<< EOS
          <a href="$imgPath" class="colorbox">
            <img src="$imgPath" align="center">
          </a>
EOS;
          $originalMatch = preg_quote($args[0]);
          drush_print_r("Original match with preg_quote is $originalMatch");

          if (preg_match('$\<span[^\>]*\>\<span class="image_meta"\>\</span\>' . $originalMatch . '\</span\>$', $value, $testargs)) {
            drush_print_r(">>>>> Match Found <<<<<");
            drush_print_r($testargs);
          }
          $value = preg_replace('$\<span [^\>]*\>\<span class="image_meta"\>\</span\>' . $originalMatch . '\</span\>$', $replacement, $value);
          drush_print_r($replacement);
          drush_print_r($value);
          
          \Drupal\Core\Database\Database::setActiveConnection();
        }


        // <a href="/sites/capetrials.co.za/files/ckutils/CottonExpert220.jpg" rel="lightbox[http://]" style="display:block;margin:0 auto"><img src="/sites/capetrials.co.za/files/ckutils/CottonExpert220_small.jpg" /></a>

        // <a class="lightbox lightbox-processed" title="A little cut" href="http://bmwmotorcycleclubcape.co.za/sites/bmwmotorcycleclubcape.co.za/files/images/wound.jpg" rel="lightbox[http://]"><img class="image image-half300 " title="A little cut" src="http://bmwmotorcycleclubcape.co.za/sites/bmwmotorcycleclubcape.co.za/files/images/wound.half300.jpg" alt="A little cut" height="225" width="300"></a>



        $row->setSourceProperty($text_field, $value);
      }
    }
    // Set the body format as full_html
    $row->setSourceProperty('format', 'full_html');
  }
}

// vim:set et sw=2 ft=php ai:
