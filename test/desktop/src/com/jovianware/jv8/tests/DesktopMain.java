package com.jovianware.jv8.tests;

public class DesktopMain {

  static JV8Tester tester;
  
  public static void main(String[] args) {
    tester = new JV8Tester();
    tester.runTest();
  }

}
