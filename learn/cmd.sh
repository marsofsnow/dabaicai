cc push action zhouhao adduser  '["bob1", 18588250437, "i m bob1"]' -p bob1@active
cc push action zhouhao adduser  '["abc",  18588250437, "i m abc"]' -p abc@active
cc push action zhouhao edituser '["bob", 18588250437, "i m bob new"]' -p bob@active
cc push action zhouhao loweruser '["bob"]' -p bob@active
cc get table zhouhao zhouhao user 
cc push action zhouhao addpost '["bob", "my post"]' -p bob
cc push action zhouhao addpost '["bob", "my post"]' -p bob