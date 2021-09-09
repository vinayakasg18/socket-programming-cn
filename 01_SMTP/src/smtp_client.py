""" 
AUTHOR: Vinayaka Gadag
CREATED: 09/09/2021
This is an smtp netcat client which connects to mail relay server """
import os
import netcat

def smtp_client():
    """ client to connect to Netcat """

    net_cat = netcat.Netcat()

    print("Connecting.....")
    net_cat.connect("mail-relay.iu.edu", 25)
    print("Connection successful!")

    print("Kind people say hello first")
    net_cat.write("Helo google.com\n")
    print(net_cat.read())

    # User inputs
    sender = input("From:")
    receiver = input("To:")
    subject = input("Subject:")
    file_name = input("File Name (file name is 'rc'):")

    # Open file and append to a string to pass the input to write()
    # Get the current directory
    # cur_dir = os.getcwd()
    dir_path = os.path.dirname(os.path.realpath(__file__))
    with open(os.path.join(dir_path, f"{file_name}" + ".txt"), 'r', encoding='utf-8') as outfile:
        lines = outfile.readlines()
        content_string = ""
        for i in lines:
            content_string = content_string + i

    # Sending all the commnads to the server
    net_cat.write("MAIL FROM:" + sender + "\n")
    print(net_cat.read())
    net_cat.write("RCPT TO:" + receiver + "\n")
    print(net_cat.read())
    net_cat.write("DATA\n")
    net_cat.write("Subject:" + subject + "\n")
    net_cat.write(content_string + "\n")
    net_cat.read()
    net_cat.write(".\n")
    print(net_cat.read())

    net_cat.close()
    print("connection closed")

if __name__ == "__main__":
    smtp_client()
