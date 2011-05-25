<?php

// $Id:$

/**
 * @file
 * Status interface
 */

interface Roots_Status {
  const STATUS_ERROR = '0';
  const STATUS_RECIEVED = '1';
  const STATUS_INPROGRESS = '2';
  const STATUS_COMPLETE = '3';

  public function getStatus();

  public function setStatus($status);
}