
# Benchmarking-Cryptographic-alogorithms-on-ESP8266-Arduino
Benchmarking Cryptographic alogorithms on ESP8266 Arduino- AES-128,192,256,  CHACHA, CHACHA Poly, Speck, Speck Tiny, Etc.....

# About our work:
 In this project, the focus is to provide an experimental benchmark study that shows the cost (e.g., processing time of encryption and decryption algorithms) of applying different security protocols on restricted devices equipped with lightweight Arduino ESP8266 sensor platform.
 <h5>The library is split into two main sections: Core and light-weight.</h5>
    
  <h2>1. Core algorithms</h2>
           
Authenticated encryption with associated data (AEAD): ChaChaPoly <br>
Block ciphers: AES128,  AES192,AES256 <br>
Stream ciphers:ChaCha <br>
Message authenticators: Poly1305 <br>
Random number generation:RNG<br>
       
  <h2>2. Light-weight algorithms </h2>
           
Authenticated encryption with associated data (AEAD): ASCON, ACRON <br>
Block ciphers: Speck,SpeckSmall,SpeckTiny <br>
