package com.jovianware.jv8.tests;

import com.jovianware.jv8.tests.JV8Tester;

import android.app.Activity;
import android.os.Bundle;

public class JV8Test extends Activity {
  
  JV8Tester tester;
  
  @Override
  public void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.main);
    
    tester = new JV8Tester();
    tester.runTest();
  }
}
