# Hk
PHP日志扩展。

#简单示例

    <?php
      Hk::setPath("/log_path");
      Hk::setInitBufferNum(3);
      Hk::test("1111111111111");
      Hk::test(json_encode(array("key"=>"val")));
      Hk::test("3333331111111");
      Hk::debug("777777111111");
      Hk::debug("888888111111");
