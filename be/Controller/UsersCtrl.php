<?php

namespace Controller;

use Service\UsersSvc;
use Slim\Http\Response;
use \Psr\Http\Message\ServerRequestInterface as Request;

class UsersCtrl
{
    /** @var UsersSvc */
    private $usersSvc;

    public function __construct(UsersSvc $usersSvc)
    {
        $this->usersSvc = $usersSvc;
    }

    public function getUsers(Response $response): Response
    {
        $userArrayList = $this->usersSvc->getUsers();
        return $response->withJson($userArrayList);
    }

    public function addUser(Request $request, Response $response): Response
    {
        $json = $request->getBody();
        $userArray = json_decode($json, true);
        
        $this->usersSvc->addUser($userArray);
        return $response->withJson(['status' => 'success']);
    }

    public function updateRatings(
        Request $request,
        Response $response
    ): Response {
        $json = $request->getBody();
        $usersCodes = json_decode($json, true);
        $winnerUserCode = $usersCodes['winnerUserCode'];
        $looserUserCode = $usersCodes['looserUserCode'];

        $ratingDiff = $this->usersSvc->updateRatings($winnerUserCode, $looserUserCode);
        return $response->withJson(['status' => 'success', 'ratingDiff' => $ratingDiff]);
    }
}
