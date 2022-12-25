<h3 align="center">Mini Linux Shell</h3>
  <p align="center">
    Mini shell made in C for Linux using GNU Readline Library
  </p>

<!-- TABLE OF CONTENTS -->
<details>
  <summary>Table of Contents</summary>
  <ol>
    <li>
      <a href="#about-the-project">About The Project</a>
      <ul>
        <li><a href="#built-with">Built With</a></li>
      </ul>
    </li>
    <li>
      <a href="#getting-started">Getting Started</a>
      <ul>
        <li><a href="#prerequisites">Prerequisites</a></li>
        <li><a href="#installation">Installation</a></li>
      </ul>
    </li>
    <li><a href="#usage">Usage</a></li>
    <li><a href="#contributing">Contributing</a></li>
    <li><a href="#license">License</a></li>
    <li><a href="#acknowledgments">Acknowledgments</a></li>
  </ol>
</details>



<!-- ABOUT THE PROJECT -->
## About The Project

* Inspired by Cornell's CS 414's Shell assignment, this project aims to emulate a shell in C, much like Csh or Bash
* Implements various advanced programming techniques such as process creation and control,
file descriptors, signals, I/O redirection and pipelines
* Takes in a list of commands separated by space or pipes and parses it into individual components such as commands, flags, I/O files, pipe and I/O operators
* Shell prompt displays the user's present working directory, user login info and their host machine name
* Final results can be neatly saved in a CSV format.
* Features 5 built-in commands: cd, history, jobs, kill and exit
* Caches user commands using GNU Readline/History library 
* Allows users to create background jobs by using the '&' operator and display them with their process IDs using the 'jobs' command
* Background jobs can be killed using 'kill' command followed by the process ID of the job
* Dynamic analysis tools such as Valgrind was used to detect and subsequently fix memory leaks


<p align="right">(<a href="#top">back to top</a>)</p>



### Built With 
* ![C](https://img.shields.io/badge/c-%2300599C.svg?style=for-the-badge&logo=c&logoColor=white)

<p align="right">(<a href="#top">back to top</a>)</p>



<!-- GETTING STARTED -->
## Getting Started
### Prerequisites

* GNU Readline Library

You can install the [GNU Readline library](https://en.wikipedia.org/wiki/GNU_Readline) GNU Readline library using the following command
  ```sh
  sudo apt-get install libreadline6 libreadline6-dev 
  ```

### Installation

1. Automatically compile the project using make,
   ```sh
   make
   ```
9. Then run using the shell by using the following command,
   ```sh
   ./shell 
   ```



<!-- USAGE EXAMPLES -->
## Usage

TODO

<!-- CONTRIBUTING -->
## Contributing

Contributions are what make the open source community such an amazing place to learn, inspire, and create. Any contributions you make are **greatly appreciated**.

If you have a suggestion that would make this better, please fork the repo and create a pull request. You can also simply open an issue with the tag "enhancement".
Don't forget to give the project a star! Thanks again!

1. Fork the Project
2. Create your Feature Branch (`git checkout -b feature/AmazingFeature`)
3. Commit your Changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to the Branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request

<p align="right">(<a href="#top">back to top</a>)</p>



<!-- LICENSE.txt -->
## License

Distributed under the MIT License. See `LICENSE.md` for more information.

<p align="right">(<a href="#top">back to top</a>)</p>

<!-- ACKNOWLEDGMENTS -->
## Acknowledgments

* [Cornell 2004 shell project template](https://www.cs.cornell.edu/courses/cs414/2004su/homework/shell/shell.html)
<p align="right">(<a href="#top">back to top</a>)</p>
