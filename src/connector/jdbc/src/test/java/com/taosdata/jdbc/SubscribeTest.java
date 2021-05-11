package com.taosdata.jdbc;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;

import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.SQLException;
import java.sql.Statement;
import java.util.Properties;
import java.util.concurrent.TimeUnit;

public class SubscribeTest {

    Connection connection;
    Statement statement;
    String dbName = "test";
    String tName = "t0";
    String host = "127.0.0.1";
    String topic = "test";

    @Test
    public void subscribe() {
        try {
            String rawSql = "select * from " + dbName + "." + tName + ";";
            TSDBConnection conn = connection.unwrap(TSDBConnection.class);
            TSDBSubscribe subscribe = conn.subscribe(topic, rawSql, false);

            int a = 0;
            while (true) {
                TimeUnit.MILLISECONDS.sleep(1000);
                TSDBResultSet resSet = subscribe.consume();
                while (resSet.next()) {
                    for (int i = 1; i <= resSet.getMetaData().getColumnCount(); i++) {
                        System.out.printf(i + ": " + resSet.getString(i) + "\t");
                    }
                    System.out.println("\n======" + a + "==========");
                }
                a++;
                if (a >= 2) {
                    break;
                }
                resSet.close();
            }

            subscribe.close(true);
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    @Before
    public void createDatabase() throws SQLException {
        Properties properties = new Properties();
        properties.setProperty(TSDBDriver.PROPERTY_KEY_CHARSET, "UTF-8");
        properties.setProperty(TSDBDriver.PROPERTY_KEY_LOCALE, "en_US.UTF-8");
        properties.setProperty(TSDBDriver.PROPERTY_KEY_TIME_ZONE, "UTC-8");
        connection = DriverManager.getConnection("jdbc:TAOS://" + host + ":0/", properties);

        statement = connection.createStatement();
        statement.execute("drop database if exists " + dbName);
        statement.execute("create database if not exists " + dbName);
        statement.execute("create table if not exists " + dbName + "." + tName + " (ts timestamp, k int, v int)");
        long ts = System.currentTimeMillis();
        statement.executeUpdate("insert into " + dbName + "." + tName + " values (" + ts + ", 100, 1)");
        statement.executeUpdate("insert into " + dbName + "." + tName + " values (" + (ts + 1) + ", 101, 2)");
    }

    @After
    public void close() {
        try {
            statement.execute("drop database " + dbName);
            if (statement != null)
                statement.close();
            if (connection != null)
                connection.close();
        } catch (SQLException e) {
            e.printStackTrace();
        }
    }
}